#include <libunittest/all.hpp>

namespace ut  = unittest;
namespace uta = ut::assertions;

#include <thread/safe_circular_buffer.hpp>

#include <chrono>
#include <thread>

namespace scb_test
{
  const unsigned long queue_size     = 100L;
  const unsigned long total_elements = queue_size * 100L;

  template<class Clock, class Duration = std::chrono::nanoseconds>
  struct basic_timer 
  {
    typedef Clock    clock_t;
    typedef Duration duration_t;

    basic_timer()
    : clock(), start( clock.now() )
    {}

    ~basic_timer() 
    {
      std::chrono::time_point<clock_t> stop = clock.now();
      std::cout << std::chrono::duration_cast<duration_t>(stop - start).count() << std::endl;
    }
    
    private:
      clock_t clock;
      std::chrono::time_point<clock_t>  start;
  };
  typedef basic_timer<std::chrono::steady_clock, std::chrono::nanoseconds> steady_timer;


  template<class Buffer>
  class Producer
  {
    typedef typename Buffer::value_type value_type;
    Buffer* m_container;

    public:
      Producer(Buffer* buffer) : m_container(buffer)
      {}

      void operator()()
      {
        for (unsigned long i = 0L; i < total_elements; ++i)
        {
          m_container->push(value_type());
        }
      }
  };

  template<class Buffer>
  class Consumer
  {
    typedef typename Buffer::value_type value_type;
    Buffer* m_container;
    value_type m_item = 0;

    public:
      Consumer(Buffer* buffer) : m_container(buffer)
      {}

      void operator()()
      {
        for (unsigned long i = 0L; i < total_elements; ++i)
        {
          m_container->pop(&m_item);
        }
      }
  };
} // scb_test

struct test_circular_buffer : ut::testcase<> 
{

  static void run()
  {
      UNITTEST_CLASS(test_circular_buffer);
      UNITTEST_RUN(basic_test);
      UNITTEST_RUN(drop_test);
  }

  test_circular_buffer() {} // executed before each test and before set_up()
  ~test_circular_buffer() {} // executed after each test and after tear_down()

  void set_up() {} // executed before each test and after constructor
  void tear_down() {} // executed after each test and before destructor

  void basic_test()
  {
    using buffer_type = thread::safe_buffer_wait<int>;
    buffer_type buf(scb_test::queue_size);

    try
    {
      for (unsigned long i = scb_test::queue_size / 2L; i > 0; --i)
      {
        buf.push( i );
      }

      scb_test::Consumer<buffer_type> consumer(&buf);
      scb_test::Producer<buffer_type> producer(&buf);

      std::thread consume(consumer);
      std::thread produce(producer);

      consume.join();
      produce.join();
    } catch(...)
    {
      uta::assert_true(false);
    }
  }

  void drop_test()
  {
    using buffer_type = thread::safe_buffer_drop<int>;
    buffer_type buf(scb_test::queue_size);

    try
    {
      for (unsigned long i = scb_test::queue_size / 2L; i > 0; --i)
      {
        buf.push( i );
      }

      scb_test::Consumer<buffer_type> consumer(&buf);
      scb_test::Producer<buffer_type> producer(&buf);

      std::thread consume(consumer);
      std::thread produce(producer);

      consume.join();
      produce.join();
    } catch(...)
    {
      uta::assert_true(false);
    }
  }

};

REGISTER(test_circular_buffer) // this registers the test class