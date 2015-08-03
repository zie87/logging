

#include <vector>
#include <iostream>

#include "sink.hpp"
#include "source.hpp"
#include "core.hpp"
#include "timer.hpp"
#include "fixed_string.hpp"

std::vector<message> message_vector;
class vector_sink : public sink
{
  public:
    using basic_type = sink;
    using size_type  = typename basic_type::size_type;
    using value_type = typename basic_type::value_type;


    explicit vector_sink(size_type size = 65536)
    : basic_type(size)
    {}

    vector_sink(const vector_sink&)            = delete;
    vector_sink& operator=(const vector_sink&) = delete;

  protected:
    void consume(const value_type& val) override { message_vector.push_back(val); }
};


void send_log_messages(source* src, unsigned int num)
{
  steady_timer timer;
  for(unsigned int i = 0; i < num; ++i) { src->log("MESSAGE"); }
}

void send_log_messages_sleep(source* src, unsigned int num, unsigned int sleep = 0)
{
  steady_timer timer;
  for(unsigned int i = 0; i < num; ++i)
  { 
    src->log(source::level_type::debug, "DEBUG MESSAGE"); 
    std::this_thread::sleep_for(std::chrono::nanoseconds(sleep));
  }
}

void print_message_vector()
{
  for(unsigned int i = 0; i < message_vector.size(); ++i) 
  { 
    std::cout << "<" << message_vector[i].channel << "> : " 
              << "[" << message_vector[i].level << "]" << " - " 
              << message_vector[i].item << std::endl; 
    // if( i%10 == 0 ) std::cout << std::endl;
  }
}

void pool_test(unsigned int iterations)
{
  std::cout << "pool_test test" << std::endl;

  message_vector.clear();
  const unsigned int worker_cnt  = 4;
  using sink_ptr = typename core::sink_ptr;

  sink_ptr  _sink( new vector_sink );
  source    sources[worker_cnt];

  auto& core_inst = core::instance();
  core_inst.insert_sink(_sink);

  std::vector<std::thread> workers;
  for( unsigned int i = 0; i < worker_cnt; ++i) 
  { 
    workers.push_back( std::thread(send_log_messages, &sources[i], (iterations/worker_cnt) ) );
  }
  for (std::thread& t: workers) { if (t.joinable()) { t.join();} }


  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  core_inst.remove_sink(_sink);
  
  std::cout << "size " << message_vector.size() << " should be " << iterations << std::endl;
  _sink->flush();

  print_message_vector();
}


void multi_sources( unsigned int iterations )
{
  std::cout << "multi_sources test" << std::endl;

  message_vector.clear();
  const unsigned int worker_cnt  = 4;
  using sink_ptr = typename core::sink_ptr;

  sink_ptr  _sink( new vector_sink );

  source sources_01("SOURCE_00");
  source sources_02("SOURCE_01");
  source sources_03("SOURCE_02");
  source sources_04("SOURCE_03");

  auto& core_inst = core::instance();
  core_inst.insert_sink(_sink);

  std::vector<std::thread> workers;

  workers.push_back( std::thread(send_log_messages_sleep, &sources_01, (iterations/worker_cnt), 10*1 ) );
  workers.push_back( std::thread(send_log_messages_sleep, &sources_02, (iterations/worker_cnt), 10*2 ) );
  workers.push_back( std::thread(send_log_messages_sleep, &sources_03, (iterations/worker_cnt), 10*3 ) );
  workers.push_back( std::thread(send_log_messages_sleep, &sources_04, (iterations/worker_cnt), 10*4 ) );

  for (std::thread& t: workers) { if (t.joinable()) { t.join();} }


  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  core_inst.remove_sink(_sink);
  
  std::cout << "size " << message_vector.size() << " should be " << iterations << std::endl;
  _sink->flush();

  print_message_vector();
}

void string_test()
{
    fixed_string_buffer<8> str1 = "foo";
    fixed_string_buffer<6> str2 = "bar";
    str1 = "foo2";
    str2 = "bar2";
    str1 = str2;
    //str2 = str1; // doesn't compile, static size of 'str1'<8> is bigger than 'str2'<6>!
    str2 = str1.c_str(); // this would assert if the actual size of 'str1'(4) is bigger than 'str2'<6>
    printf("%s %s\n", str1.c_str(), str2.c_str());

    fixed_string_buffer<20> msg;
    msg.format("%s %s 0123456789", "Hello", "World!"); // truncated to 'Hello World! 012345'
    printf("'%s'\n", msg.c_str());
}


int main()
{
  const unsigned int iterations  = 100000;
  
  string_test();

  pool_test(iterations);
  multi_sources(iterations);


  return 0;
}