#include <libunittest/all.hpp>

namespace ut  = unittest;
namespace uta = ut::assertions;

#include <factory_blog/dynamic_observer/distributor.hpp>
#include <factory_blog/dynamic_observer/console_observer.hpp>

namespace fbdo = ::factory_blog::dynamic_observer;

namespace sdo
{
  using level_type = typename fbdo::message::level_type;

  std::vector<fbdo::observer::message_type> messages;

  struct test_observer : public fbdo::observer
  {
    using level_type   = typename fbdo::observer::level_type;
    using message_type = typename fbdo::observer::message_type;

    test_observer( const level_type l )
    : fbdo::observer(l)
    {}

    ~test_observer() { messages.clear(); }

    void consume( const message_type& msg ) const override
    {
      messages.push_back(msg);
    }
    

  };

} // sdo

struct sdo_test : ut::testcase<> 
{

  static void run()
  {
    UNITTEST_CLASS(sdo_test);
    UNITTEST_RUN(simple_test);
  }

  sdo_test() {} // executed before each test and before set_up()
  ~sdo_test() {} // executed after each test and after tear_down()

  void set_up() {} // executed before each test and after constructor
  void tear_down() {} // executed after each test and before destructor

  void simple_test()
  {
    fbdo::distributor& dist = fbdo::distributor::instance();

    dist += new sdo::test_observer( sdo::level_type::debug );

    sdo::messages.clear();
    dist.log( fbdo::message("test", sdo::level_type::debug, "TEST MESSAGE!") );
    uta::assert_equal( 1, sdo::messages.size() );

    for(unsigned int i = 1; i < 100; ++i)
    {
      dist.log( fbdo::message("test", sdo::level_type::debug, "TEST MESSAGE!") );
    } 
    uta::assert_equal( 100, sdo::messages.size() );

    dist -= dist.get_observer(0);

    dist += new fbdo::console_observer( sdo::level_type::error );
    for(unsigned int i = 1; i < 10; ++i)
    {
      dist.log( fbdo::message("console", sdo::level_type::debug, "TEST DEBUG MESSAGE!") );
      dist.log( fbdo::message("console", sdo::level_type::trace, "TEST TRACE MESSAGE!") );
      dist.log( fbdo::message("console", sdo::level_type::event, "TEST EVENT MESSAGE!") );
      dist.log( fbdo::message("console", sdo::level_type::error, "TEST ERROR MESSAGE!") );
    } 

  }
};

REGISTER(sdo_test) // this registers the test class