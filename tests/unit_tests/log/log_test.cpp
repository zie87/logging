
#include <memory>
#include <vector>


#include <log/message.hpp>
#include <log/distributor.hpp>
#include <log/observer.hpp>

namespace log_test
{

  class test_observer : public ::logging::observer
  {
    public:
      using observer_type = ::logging::observer;
      using message_type  = typename observer_type::message_type;

      test_observer()  
      : observer_type(), messages()
      {}

      test_observer(const test_observer&)             = delete;
      test_observer& operator=(const test_observer&)  = delete;

      std::vector<message_type> messages;

    protected:
      virtual void write(const message_type& msg)
      {
        messages.push_back(msg);
      }
  };
}

using message_type     = logging::message;
using distributor_type = logging::distributor;
using observer_type    = log_test::test_observer;

using level_type   = message_type::level;
using string_type  = typename message_type::string_type;
using channel_type = typename message_type::channel_type;


#include <libunittest/all.hpp>

namespace ut  = unittest;
namespace uta = ut::assertions;

struct test_logging : ut::testcase<> 
{

  static void run()
  {
      UNITTEST_CLASS(test_logging);
      UNITTEST_RUN(simple_test);
  }

  test_logging() {} // executed before each test and before set_up()
  ~test_logging() {} // executed after each test and after tear_down()

  void set_up() {} // executed before each test and after constructor
  void tear_down() {} // executed after each test and before destructor

  void simple_test()
  {
    std::vector<message_type> message_vec;

    message_vec.push_back(message_type( "simple", 6,  level_type::trace,   "This is a trace-log!"  , 20 ));
    message_vec.push_back(message_type( "simple", 6,  level_type::debug,   "This is a debug-log!"  , 20 ));
    message_vec.push_back(message_type( "simple", 6,  level_type::info,    "This is an info-log!"  , 20 ));
    message_vec.push_back(message_type( "simple", 6,  level_type::warn,    "This is a warning-log!", 22 ));
    message_vec.push_back(message_type( "simple", 6,  level_type::error,   "This is an error-log!" , 21 ));
 
    // std::shared_ptr<observer_type> observer( new observer_type() );
    // distributor_type::instance().append_observer( observer.get() );

    // for( auto&& msg : message_vec ) { distributor_type::instance().log(msg); }

    // wait for processing core didn't block
    // std::this_thread::sleep_for(std::chrono::milliseconds(10));
    // distributor_type::instance().remove_observer(observer.get());

    // uta::assert_equal( message_vec.size(), observer->messages.size() );
    // for( unsigned int i = 0; i < message_vec.size(); ++i )
    // {
    //   uta::assert_true( message_vec[i].get_level()   == observer->messages[i].get_level() );
    //   uta::assert_true( message_vec[i].get_channel() == observer->messages[i].get_channel() );
    //   uta::assert_true( message_vec[i].get_string()  == observer->messages[i].get_string() );
    // }
  }
};

REGISTER(test_logging) // this registers the test class