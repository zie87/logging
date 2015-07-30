#include <libunittest/all.hpp>

namespace ut  = unittest;
namespace uta = ut::assertions;

#include <factory_blog/async_observer/message_queue.hpp>

namespace fbso = ::factory_blog::async_observer;

#include <thread>
#include <sstream>
#include <vector>

namespace ado_mq
{
  const unsigned long queue_size     = 100L;
  const unsigned long total_elements = queue_size * 100L;

  template<class Buffer>
  class producer
  {
    using buffer_type  = Buffer;
    using message_type = typename buffer_type::message_type;

    using string_type  = typename message_type::string_type;
    using channel_type = typename message_type::channel_type;
    using level_type   = typename message_type::level_type;


    buffer_type*  m_container;
    channel_type  m_channel;
    unsigned int  m_sleep;

    public:
      producer(Buffer* buffer, unsigned int sleep_time = 1) : m_container(buffer), m_channel(), m_sleep(sleep_time)
      {
        auto myid = std::this_thread::get_id();
        std::stringstream ss; ss << myid;
        m_channel = ss.str();
      }

      producer(const producer& p)
      : m_container(p.m_container), m_channel(p.m_channel), m_sleep(p.m_sleep)
      {}


      producer& operator=(const producer&) = delete;

      void operator()()
      {
        for (unsigned long i = 0L; i < total_elements; ++i)
        {
          m_container->push( fbso::message(m_channel, level_type::trace, "TEST MESSAGE" ) );
          std::this_thread::sleep_for(std::chrono::nanoseconds(m_sleep));
        }
      }
  };

} // sdo

struct message_queue_test : ut::testcase<> 
{

  static void run()
  {
    UNITTEST_CLASS(message_queue_test);
    UNITTEST_RUN(simple_test);
  }

  message_queue_test() {} // executed before each test and before set_up()
  ~message_queue_test() {} // executed after each test and after tear_down()

  void set_up() {} // executed before each test and after constructor
  void tear_down() {} // executed after each test and before destructor

  void simple_test()
  {
    using buffer_type  = fbso::message_queue;
    using message_type = typename buffer_type::message_type;
    using thread_pool  = std::vector<std::thread>; 

    buffer_type buf;

    const unsigned int number_of_threads = 8;
    thread_pool producers;

    for( unsigned int i = 0; i < number_of_threads; ++i ) producers.push_back( std::thread( ado_mq::producer<buffer_type>(&buf, i*5) ) );

    try
    {
      for (unsigned long i = ado_mq::queue_size / 2L; i > 0; --i)
      {
        buf.push( message_type( "test filter", message_type::level_type::debug, "TEST DEBUG MESSAGE!" ) );
      }

      for( auto& p : producers )
      {
        if(p.joinable()) { p.join(); }
      }

    } catch(...)
    {
      uta::assert_true(false);
    }
  }
};

REGISTER(message_queue_test) // this registers the test class