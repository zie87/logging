#include <libunittest/all.hpp>

namespace ut  = unittest;
namespace uta = ut::assertions;

#include <thread>
#include <future>
#include <string>

#include <queues/mpmc_buffer.hpp>

#include "queues_test.hpp"

namespace mpmc_buffer_test_ns
{
  using queue_type          = ::mpmc_buffer<::queues_test::size_type>;
  const auto& consumer_type = ::queues_test::consume<queue_type>;
  const auto& producer_type = ::queues_test::buffer_produce<queue_type>;

  const queues_test::size_type queue_size = 65536;
}


struct mpmc_buffer_test : ut::testcase<> 
{

  static void run()
  {
    UNITTEST_CLASS(mpmc_buffer_test);
    
    UNITTEST_RUN(single_producer_single_consumer);
    UNITTEST_RUN(single_producer_multi_consumer);
    UNITTEST_RUN(multi_producer_multi_consumer);
    UNITTEST_RUN(multi_producer_single_consumer);
    UNITTEST_RUN(char_ptr_buffer);
    // UNITTEST_RUN(string_buffer);
  }

  mpmc_buffer_test()  {} // executed before each test and before set_up()
  ~mpmc_buffer_test() {} // executed after each test and after tear_down()

  void set_up()    {} // executed before each test and after constructor
  void tear_down() {} // executed after each test and before destructor

  void single_producer_single_consumer()
  {
    using namespace mpmc_buffer_test_ns;
    queue_type q(queue_size);

    queue_type::value_type val_array[queue_size];
    for(unsigned int i = 0; i < queue_size; ++i) { val_array[i] = i; }

    auto c_future  = std::async(std::launch::async, &consumer_type, std::ref(q), queue_size);
    auto p_made    = producer_type( std::ref(q), queue_size, val_array );

    auto consumed = c_future.get();

    uta::assert_equal( queue_size, p_made   );
    uta::assert_equal( queue_size, consumed );
  }

  void single_producer_multi_consumer()
  {
    using namespace mpmc_buffer_test_ns;
    queue_type q(queue_size);

    queue_type::value_type val_array[queue_size];
    for(unsigned int i = 0; i < queue_size; ++i) { val_array[i] = i; }

    auto c1_future  = std::async(std::launch::async, &consumer_type, std::ref(q), (queue_size/4) );
    auto c2_future  = std::async(std::launch::async, &consumer_type, std::ref(q), (queue_size/4) );
    auto c3_future  = std::async(std::launch::async, &consumer_type, std::ref(q), (queue_size/4) );
    auto c4_future  = std::async(std::launch::async, &consumer_type, std::ref(q), (queue_size/4) );

    auto p_made    = producer_type( std::ref(q), queue_size, val_array );

    auto c1 = c1_future.get();
    auto c2 = c2_future.get();
    auto c3 = c3_future.get();
    auto c4 = c4_future.get();

    auto consumed = c1 + c2 + c3 + c4;

    uta::assert_equal( queue_size, p_made   );
    uta::assert_equal( queue_size, consumed );
  }

  void multi_producer_multi_consumer()
  {
    using namespace mpmc_buffer_test_ns;
    queue_type q(queue_size);

    const unsigned short producer_cnt = 4;
    queue_type::value_type val_array[producer_cnt][queue_size/producer_cnt];
    
    for(unsigned short j = 0; j < producer_cnt; ++j)
    {
      for(unsigned int i = 0; i < queue_size/producer_cnt; ++i) { val_array[j][i] = i; }
    }

    auto c1_future  = std::async(std::launch::async, &consumer_type, std::ref(q), (queue_size/4) );
    auto c2_future  = std::async(std::launch::async, &consumer_type, std::ref(q), (queue_size/4) );
    auto c3_future  = std::async(std::launch::async, &consumer_type, std::ref(q), (queue_size/4) );
    auto c4_future  = std::async(std::launch::async, &consumer_type, std::ref(q), (queue_size/4) );

    auto p1_future  = std::async(std::launch::async, &producer_type, std::ref(q), (queue_size/4), val_array[0] );
    auto p2_future  = std::async(std::launch::async, &producer_type, std::ref(q), (queue_size/4), val_array[1] );
    auto p3_future  = std::async(std::launch::async, &producer_type, std::ref(q), (queue_size/4), val_array[2] );
    auto p4_future  = std::async(std::launch::async, &producer_type, std::ref(q), (queue_size/4), val_array[3] );

    auto p1 = p1_future.get();
    auto p2 = p2_future.get();
    auto p3 = p3_future.get();
    auto p4 = p4_future.get();

    auto c1 = c1_future.get();
    auto c2 = c2_future.get();
    auto c3 = c3_future.get();
    auto c4 = c4_future.get();

    auto produced = p1 + p2 + p3 + p4;
    auto consumed = c1 + c2 + c3 + c4;

    uta::assert_equal( queue_size, produced );
    uta::assert_equal( queue_size, consumed );
  }

  void multi_producer_single_consumer()
  {
    using namespace mpmc_buffer_test_ns;
    queue_type q(queue_size);

    const unsigned short producer_cnt = 4;
    queue_type::value_type val_array[producer_cnt][queue_size/producer_cnt];
    
    for(unsigned short j = 0; j < producer_cnt; ++j)
    {
      for(unsigned int i = 0; i < queue_size/producer_cnt; ++i) { val_array[j][i] = i; }
    }

    auto c_future  = std::async(std::launch::async, &consumer_type, std::ref(q), queue_size);

    auto p1_future  = std::async(std::launch::async, &producer_type, std::ref(q), (queue_size/4), val_array[0] );
    auto p2_future  = std::async(std::launch::async, &producer_type, std::ref(q), (queue_size/4), val_array[1] );
    auto p3_future  = std::async(std::launch::async, &producer_type, std::ref(q), (queue_size/4), val_array[2] );
    auto p4_future  = std::async(std::launch::async, &producer_type, std::ref(q), (queue_size/4), val_array[3] );

    auto p1 = p1_future.get();
    auto p2 = p2_future.get();
    auto p3 = p3_future.get();
    auto p4 = p4_future.get();

    auto consumed = c_future.get();
    auto produced = p1 + p2 + p3 + p4;

    uta::assert_equal( queue_size, produced );
    uta::assert_equal( queue_size, consumed );
  }

  void char_ptr_buffer()
  {
    using namespace mpmc_buffer_test_ns;
    using string_queue = ::mpmc_buffer<const char*>;
    string_queue q(queue_size);

    string_queue::value_type val_array[queue_size];
    for(unsigned int i = 0; i < queue_size; ++i) { val_array[i] = "test"; }

    auto c_future  = std::async(std::launch::async, &::queues_test::consume<string_queue>, std::ref(q), queue_size);
    auto p_made    = ::queues_test::buffer_produce<string_queue>( std::ref(q), queue_size, val_array );

    auto consumed = c_future.get();

    uta::assert_equal( queue_size, p_made   );
    uta::assert_equal( queue_size, consumed );
  }

  // void string_buffer()
  // {
  //   using namespace mpmc_buffer_test_ns;
  //   using string_queue = ::mpmc_buffer<std::string>;
  //   string_queue q(queue_size);

  //   string_queue::value_type val_array[queue_size];
  //   for(unsigned int i = 0; i < queue_size; ++i) { val_array[i] = "test"; }

  //   auto c_future  = std::async(std::launch::async, &::queues_test::consume<string_queue>, std::ref(q), queue_size);
  //   auto p_made    = ::queues_test::buffer_produce<string_queue>( std::ref(q), queue_size, val_array );

  //   auto consumed = c_future.get();

  //   uta::assert_equal( queue_size, p_made   );
  //   uta::assert_equal( queue_size, consumed );
  // }
};

REGISTER(mpmc_buffer_test) // this registers the test class
