#ifndef TEST_IMPL_SOURCE_HPP
#define TEST_IMPL_SOURCE_HPP

#include <mutex>
#include <thread>
#include <future>
#include <atomic>
#include <condition_variable>

#include <queues/mpmc_buffer.hpp>

#include "sink.hpp"
#include "core.hpp"
#include "message.hpp"

class source
{
  public:
    using value_type       = message;
    using channel_type     = typename value_type::channel_type;
    using string_type      = typename value_type::string_type;
    using level_type       = typename value_type::severity_level;

    using container_type  = ::mpmc_buffer<value_type>;
    using size_type       = typename container_type::size_type;
    using mutex_type      = std::recursive_mutex;

    explicit source(const channel_type& name = "default", size_type size = 65536)
    : m_container(size), m_mutex()
    , m_stop_process(true), m_process_future()
    , m_name(name)
    {}

    virtual ~source()
    {
      stop();
      value_type item;
      while( m_container.pop( item ) ){;}
    }

    // source& operator=( source&& src )
    // {
    //   m_container = std::move(src.m_container);
    //   m_stop_process.store( src.m_stop_process.load() );
    //   // m_process_future 
    //   m_name = std::move( src.m_name );

    //   return *this;
    // }

    source(const source&)            = delete;
    source& operator=(const source&) = delete;

    void flush()
    {
      std::unique_lock<mutex_type> lock(m_mutex);
      value_type item;
      while( m_container.pop(item) ){ consume(item); }
      lock.unlock();
    }

    void log(const value_type& val)
    {
      // std::unique_lock<mutex_type> lock(m_mutex);
      start();
      // while(! m_container.push(val) ) {;}
      m_container.push( val );
      // lock.unlock();
    }

    void log(value_type&& val)
    {
      // std::unique_lock<mutex_type> lock(m_mutex);
      start();
      // while(! m_container.push(val) ) {;}
      m_container.push( std::move(val) );
      // lock.unlock();
    }

    void log(const string_type& i) { log( std::move( value_type(m_name, i) ) ); }
    void log(level_type l, const string_type& i) { log( std::move( value_type(m_name, i, l) ) ); }

    void start()
    {
      if( m_stop_process.load(std::memory_order_acquire) )
      {
        m_stop_process.store(false, std::memory_order_release);
        m_process_future = std::move( std::async(std::launch::async, &source::process, this) );
      }
    }

    void stop()
    {
      if( !m_stop_process.load(std::memory_order_acquire) )
      {  
        m_stop_process.store(true, std::memory_order_release);
        if(m_process_future.valid())
        {
          m_process_future.get();
        }
      }
    }

    void process()
    {
      static std::mutex mtx;
      while( !m_stop_process.load(std::memory_order_acquire)  )
      {
        std::unique_lock<std::mutex> lock(mtx);
        value_type val; 
        if(m_container.pop(val))
        {
          consume(std::move(val));
        }
        lock.unlock();
      }
      m_stop_process.store(true, std::memory_order_release);
    }


  private:
    void consume(const value_type& val) { core::instance().transfer(val); }
    void consume(value_type&& val) { core::instance().transfer(std::move(val)); }

    container_type          m_container;
    mutex_type              m_mutex;

    std::atomic<bool>       m_stop_process;
    std::future<void>       m_process_future;

    channel_type             m_name;
};

#endif // TEST_IMPL_SOURCE_HPP