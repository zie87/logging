#include <libunittest/all.hpp>

namespace ut  = unittest;
namespace uta = ut::assertions;

#include <thread>
#include <future>

#include <queues/spsc_queue.hpp>

#include "queues_test.hpp"

namespace spsc_queue_test_ns
{
  using queue_type          = ::spsc_queue<::queues_test::size_type>;
  const auto& consumer_type = ::queues_test::consume<queue_type>;
  const auto& producer_type = ::queues_test::queue_produce<queue_type>;
}


struct spsc_queue_test : ut::testcase<> 
{

  static void run()
  {
    UNITTEST_CLASS(spsc_queue_test);
    
    UNITTEST_RUN(single_producer_single_consumer);
  }

  spsc_queue_test() {} // executed before each test and before set_up()
  ~spsc_queue_test() {} // executed after each test and after tear_down()

  void set_up() {} // executed before each test and after constructor
  void tear_down() {} // executed after each test and before destructor

  void single_producer_single_consumer()
  {
    using namespace spsc_queue_test_ns;
    const queues_test::size_type queue_size = 65536;
    queue_type q;

    queue_type::value_type val_array[queue_size];
    for(unsigned int i = 0; i < queue_size; ++i) { val_array[i] = i; }

    auto c_future  = std::async(std::launch::async, &consumer_type, std::ref(q), queue_size);
    auto p_made    = producer_type( std::ref(q), queue_size, val_array );

    auto consumed = c_future.get();

    uta::assert_equal( queue_size, p_made   );
    uta::assert_equal( queue_size, consumed );
  }
};

REGISTER(spsc_queue_test) // this registers the test class
