#include <libunittest/all.hpp>

namespace ut  = unittest;
namespace uta = ut::assertions;

#include <thread>
#include <future>

#include <queues/mpsc_queue.hpp>

#include "queues_test.hpp"

namespace mpsc_queue_test_ns
{
  using queue_type          = ::mpsc_queue<::queues_test::size_type>;
  const auto& consumer_type = ::queues_test::consume<queue_type>;
  const auto& producer_type = ::queues_test::produce2<queue_type>;
}


struct mpsc_queue_test : ut::testcase<> 
{

  static void run()
  {
    UNITTEST_CLASS(mpsc_queue_test);
    
    UNITTEST_RUN(single_producer_single_consumer);
    UNITTEST_RUN(multi_producer_single_consumer);
  }

  mpsc_queue_test() {} // executed before each test and before set_up()
  ~mpsc_queue_test() {} // executed after each test and after tear_down()

  void set_up() {} // executed before each test and after constructor
  void tear_down() {} // executed after each test and before destructor

  void single_producer_single_consumer()
  {
    using namespace mpsc_queue_test_ns;
    const queues_test::size_type queue_size = 65536;
    queue_type q;

    auto c_future  = std::async(std::launch::async, &consumer_type, std::ref(q), queue_size);
    auto p_made    = producer_type( std::ref(q), queue_size );

    auto consumed = c_future.get();


    uta::assert_equal( queue_size, p_made   );
    uta::assert_equal( queue_size, consumed );
  }

  void multi_producer_single_consumer()
  {
    using namespace mpsc_queue_test_ns;
    const queues_test::size_type queue_size = 65536;
    queue_type q;

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

REGISTER(mpsc_queue_test) // this registers the test class
