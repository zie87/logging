
#include <memory>
#include <vector>


#include <log/source.hpp>
#include <log/message.hpp>

using source_type = ::logging::source;
using level_type  = typename ::logging::message::level_type;

#include <libunittest/all.hpp>

namespace ut  = unittest;
namespace uta = ut::assertions;

struct test_source : ut::testcase<> 
{

  static void run()
  {
      UNITTEST_CLASS(test_source);
      UNITTEST_RUN(simple_test);
  }

  test_source() {} // executed before each test and before set_up()
  ~test_source() {} // executed after each test and after tear_down()

  void set_up() {} // executed before each test and after constructor
  void tear_down() {} // executed after each test and before destructor

  void simple_test()
  {
    source_type logger;

    logger.log( level_type::info, "first test string", 17 );

    // for(unsigned int i = 0; i < 1000; ++i )
    // {
    //   logger.log( level_type::info, "test string", 11 );
    // }

    // logger.log( level_type::info, "first test string", 17 );
    // logger.log( level_type::info, "first test string", 17 );
    // logger.log( level_type::info, "first test string", 17 );
    // logger.log( level_type::info, "first test string", 17 );
    // logger.log( level_type::info, "first test string", 17 );
    // logger.log( level_type::info, "first test string", 17 );
    // logger.log( level_type::info, "first test string", 17 );
    // logger.log( level_type::info, "first test string", 17 );

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
};

REGISTER(test_source) // this registers the test class