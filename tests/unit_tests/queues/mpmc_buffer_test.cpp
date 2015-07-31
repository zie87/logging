#include <libunittest/all.hpp>

namespace ut  = unittest;
namespace uta = ut::assertions;

#include <thread>
#include <future>

#include <queues/mpmc_buffer.hpp>

#include "queues_test.hpp"

namespace mpmc_buffer_test_ns
{
  using queue_type          = ::mpmc_buffer<::queues_test::size_type>;
  const auto& consumer_type = ::queues_test::consume<queue_type>;
  const auto& producer_type = ::queues_test::produce<queue_type>;

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
  }

  mpmc_buffer_test()  {} // executed before each test and before set_up()
  ~mpmc_buffer_test() {} // executed after each test and after tear_down()

  void set_up()    {} // executed before each test and after constructor
  void tear_down() {} // executed after each test and before destructor

  void single_producer_single_consumer()
  {
    using namespace mpmc_buffer_test_ns;
    queue_type q(queue_size);

    auto c_future  = std::async(std::launch::async, &consumer_type, std::ref(q), queue_size);
    auto p_made    = producer_type( std::ref(q), queue_size );

    auto consumed = c_future.get();

    uta::assert_equal( queue_size, p_made   );
    uta::assert_equal( queue_size, consumed );
  }

  void single_producer_multi_consumer()
  {
    using namespace mpmc_buffer_test_ns;
    queue_type q(queue_size);

    auto c1_future  = std::async(std::launch::async, &consumer_type, std::ref(q), (queue_size/4) );
    auto c2_future  = std::async(std::launch::async, &consumer_type, std::ref(q), (queue_size/4) );
    auto c3_future  = std::async(std::launch::async, &consumer_type, std::ref(q), (queue_size/4) );
    auto c4_future  = std::async(std::launch::async, &consumer_type, std::ref(q), (queue_size/4) );

    auto p_made    = producer_type( std::ref(q), queue_size );

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

    auto c1_future  = std::async(std::launch::async, &consumer_type, std::ref(q), (queue_size/4) );
    auto c2_future  = std::async(std::launch::async, &consumer_type, std::ref(q), (queue_size/4) );
    auto c3_future  = std::async(std::launch::async, &consumer_type, std::ref(q), (queue_size/4) );
    auto c4_future  = std::async(std::launch::async, &consumer_type, std::ref(q), (queue_size/4) );

    auto p1_future  = std::async(std::launch::async, &producer_type, std::ref(q), (queue_size/4) );
    auto p2_future  = std::async(std::launch::async, &producer_type, std::ref(q), (queue_size/4) );
    auto p3_future  = std::async(std::launch::async, &producer_type, std::ref(q), (queue_size/4) );
    auto p4_future  = std::async(std::launch::async, &producer_type, std::ref(q), (queue_size/4) );

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

    auto c_future  = std::async(std::launch::async, &consumer_type, std::ref(q), queue_size);

    auto p1_future  = std::async(std::launch::async, &producer_type, std::ref(q), (queue_size/4) );
    auto p2_future  = std::async(std::launch::async, &producer_type, std::ref(q), (queue_size/4) );
    auto p3_future  = std::async(std::launch::async, &producer_type, std::ref(q), (queue_size/4) );
    auto p4_future  = std::async(std::launch::async, &producer_type, std::ref(q), (queue_size/4) );

    auto p1 = p1_future.get();
    auto p2 = p2_future.get();
    auto p3 = p3_future.get();
    auto p4 = p4_future.get();

    auto consumed = c_future.get();
    auto produced = p1 + p2 + p3 + p4;

    uta::assert_equal( queue_size, produced );
    uta::assert_equal( queue_size, consumed );
  }
};

REGISTER(mpmc_buffer_test) // this registers the test class
