
#include "message.hpp"
#include "source.hpp"
#include "sink.hpp"
#include "core.hpp"
#include "fixed_string.hpp"

#include "timer.hpp"

#include <iostream>
#include <iomanip>

#include <cmath>

#include <boost/log/common.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/utility/empty_deleter.hpp>


namespace bo_log = boost::log;

class boost_worker
{
  public:
    using channel_type = typename message::channel_type;
    using level_type   = typename message::severity_level;

    boost_worker(const channel_type& name)
    : m_logger( boost::log::keywords::channel = name )
    {}

    virtual ~boost_worker() {}
  
    virtual void work() 
    {
      trace( __FUNCTION__, "start work!" );
      log(level_type::debug,     "for your information");
      log(level_type::info,     "for your information");
      log(level_type::warning,  "WARNING WARNING!" );
      log(level_type::error,    "THIS IS NOT GOOD!" );
      log(level_type::critical, "THE SYSTEM IS BROKEN !!!!" );
    }

    virtual void work(unsigned int cnt) { for( unsigned int i = 0; i < cnt; ++i ) work(); }

  protected:
    inline void log( level_type lvl, const char* msg )
    {
      namespace blog = boost::log; 
      blog::record rec = m_logger.open_record(blog::keywords::severity = lvl);
      if(rec) { blog::aux::make_record_pump(m_logger, rec).stream() << msg; }
    }

    inline void trace(const char* name, const char* msg)
    {
      namespace blog = boost::log; 
      blog::record rec = m_logger.open_record(blog::keywords::severity = level_type::trace);
      if(rec) { blog::aux::make_record_pump(m_logger, rec).stream() << name << ": " << msg; }
    }

    typedef boost::log::sources::severity_channel_logger_mt<level_type, channel_type> logger_t;
    logger_t m_logger;
};


class test_worker
{
  public:
    using channel_type = typename message::channel_type;
    using level_type   = typename message::severity_level;
    using string_type  = typename message::string_type;

    test_worker(const channel_type& name)
    : m_logger(name.c_str())
    {}

    virtual ~test_worker() {}
  
    virtual void work() 
    {
      trace( __FUNCTION__, "start work!" );
      log(level_type::debug,     "for your information");
      log(level_type::info,     "for your information");
      log(level_type::warning,  "WARNING WARNING!" );
      log(level_type::error,    "THIS IS NOT GOOD!" );
      log(level_type::critical, "THE SYSTEM IS BROKEN !!!!" );
    }

    virtual void work(unsigned int cnt) { for( unsigned int i = 0; i < cnt; ++i ) work(); }

  protected:
    inline void log( level_type lvl, const char* msg )
    {
      m_logger.log( lvl, msg );
    }

    inline void trace(const char* name, const char* msg)
    {
      string_type str;
      str.format("%s : %s", name, msg);
      m_logger.log( level_type::trace, str.c_str() );
    }

    source m_logger;
};

class output_sink : public sink
{
  public:
    using basic_type = sink;
    using size_type  = typename basic_type::size_type;
    using value_type = typename basic_type::value_type;


    explicit output_sink(size_type size = 65536)
    : basic_type(size)
    {}

    output_sink(const output_sink&)            = delete;
    output_sink& operator=(const output_sink&) = delete;

  protected:
    void consume(const value_type& val) override 
    { 
      std::cout << "<" << val.channel.c_str() << "> : " 
                << "[" << static_cast<int>(val.level) << "]" << " - " 
                << val.item.c_str() << std::endl; 
    }
};


static std::vector<size_t> time_vector;

size_t measurement_variance_calculation(const size_t& avg )
{
  size_t sum = 0;
  for(unsigned int i = 0; i < time_vector.size(); ++ i)
  {
    size_t current = time_vector[i] - avg;
    sum += current*current;
  }

  size_t variance = sum/( time_vector.size() -1 );
  return variance; 
}

void print_time_vector( const std::string& name )
{
  if( time_vector.empty() ) return;
  size_t max = 0;
  size_t min = time_vector[0];
  size_t sum = 0;
  size_t avg = 0;

  unsigned long int vec_size = time_vector.size();

  for(unsigned long int i = 0; i < vec_size; ++ i)
  {
    size_t current = time_vector[i];
    if( current < min ) min = current;
    if( current > max ) max = current;
    sum += current;
  }

  avg = sum / vec_size;
  size_t dev = sqrt( measurement_variance_calculation(avg) );
  size_t err = dev/sqrt(vec_size);

  std::clog << std::endl;
  std::clog << name << std::setw(30 - name.size() ) << "min time:\t" << std::right << std::setw(10) << min << std::endl;
  std::clog << name << std::setw(30 - name.size() ) << "max time:\t" << std::right << std::setw(10) << max << std::endl;
  std::clog << name << std::setw(30 - name.size() ) << "avg time:\t" << std::right << std::setw(10) << avg << std::endl;
  std::clog << name << std::setw(30 - name.size() ) << "dev time:\t" << std::right << std::setw(10) << dev << std::endl;
  std::clog << name << std::setw(30 - name.size() ) << "err time:\t" << std::right << std::setw(10) << err << std::endl;
  std::clog << std::endl;
}


template<class W>
void logging_function(W& worker, const unsigned int cnt = 10)
{
  steady_vector_timer t(time_vector);

  // auto worker_threads = std::thread( std::bind(&W::work, worker, cnt) );
  auto worker_threads = std::thread( std::bind( static_cast<void(W::*)(unsigned int)>(&W::work), &worker, cnt ) );
  worker_threads.join();
}


typedef  bo_log::sinks::text_ostream_backend                                         backend_t;
typedef  bo_log::sinks::bounded_fifo_queue< 65536, bo_log::sinks::drop_on_overflow > queue_t;
typedef  bo_log::sinks::asynchronous_sink< backend_t, queue_t >                      async_sink_t;

int main()
{
  boost_worker b_worker("boost");
  test_worker  t_worker("test");

  using sink_ptr = typename core::sink_ptr;

  sink_ptr  t_sink( new output_sink );

  auto& core_inst = core::instance();
  core_inst.insert_sink(t_sink);

  unsigned int repeations = 100;
  unsigned int counter    = 1000;

  time_vector.clear();
  for( unsigned int i=0; i < repeations; ++i  ) { logging_function(t_worker, counter); }
  print_time_vector("(T) logging_function ");

  core_inst.remove_sink(t_sink);
  t_sink->flush();


  std::this_thread::sleep_for(std::chrono::milliseconds(5000));


  typedef  boost::shared_ptr< async_sink_t >  sink_ptr_t;
  sink_ptr_t cmd_sink( boost::make_shared< async_sink_t >() );

  boost::shared_ptr<std::ostream> stream(&std::cout, bo_log::empty_deleter());
  cmd_sink->locked_backend()->add_stream( stream );
  bo_log::core::get()->add_sink( cmd_sink );

  time_vector.clear();
  for( unsigned int i=0; i < repeations; ++i  ) { logging_function(b_worker, counter); }
  print_time_vector("(B) logging_function ");

  return 0;
}