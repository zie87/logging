#include <libunittest/all.hpp>

namespace ut  = unittest;
namespace uta = ut::assertions;

#include <factory_blog/async_observer/distributor.hpp>
#include <factory_blog/async_observer/console_observer.hpp>

namespace fbso = ::factory_blog::async_observer;

namespace ado
{
  using level_type = typename fbso::message::level_type;

  std::vector<fbso::observer::message_type> messages;

  struct test_observer : public fbso::observer
  {
    using level_type   = typename fbso::observer::level_type;
    using message_type = typename fbso::observer::message_type;

    test_observer( const level_type l )
    : fbso::observer(l)
    {}

    ~test_observer() { messages.clear(); }

    protected:
      void consume( const message_type& msg ) const override{ messages.push_back(msg); }
  };

} // ado

struct ado_test : ut::testcase<> 
{

  static void run()
  {
    UNITTEST_CLASS(ado_test);
    UNITTEST_RUN(simple_test);
  }

  ado_test() {} // executed before each test and before set_up()
  ~ado_test() {} // executed after each test and after tear_down()

  void set_up() {} // executed before each test and after constructor
  void tear_down() {} // executed after each test and before destructor

  void simple_test()
  {
    fbso::distributor& dist = fbso::distributor::instance();

    fbso::observer* obs = new fbso::console_observer( ado::level_type::debug );
    // fbso::observer* obs = new ado::test_observer( ado::level_type::debug );

    dist += obs;


    // dist += new ado::test_observer( ado::level_type::debug );

    // ado::messages.clear();
    dist.log( fbso::message("test", ado::level_type::debug, "TEST MESSAGE!") );

    // uta::assert_equal( 1, ado::messages.size() );

    for(unsigned int i = 1; i < 100; ++i)
    {
      dist.log( fbso::message("test", ado::level_type::debug, "TEST MESSAGE!") );
    } 


    // std::this_thread::sleep_for(std::chrono::milliseconds(10));
    // uta::assert_equal( 100, ado::messages.size() );

    obs->stop();
    // std::this_thread::sleep_for(std::chrono::milliseconds(10));
    // dist -= dist.get_observer(0);

    // ado::messages.clear();
    // dist += new ado::test_observer( ado::level_type::error );
    // for(unsigned int i = 0; i < 10; ++i)
    // {
    //   dist.log( fbso::message("test filter", ado::level_type::debug, "TEST DEBUG MESSAGE!") );
    //   dist.log( fbso::message("test filter", ado::level_type::trace, "TEST TRACE MESSAGE!") );
    //   dist.log( fbso::message("test filter", ado::level_type::event, "TEST EVENT MESSAGE!") );
    //   dist.log( fbso::message("test filter", ado::level_type::error, "TEST ERROR MESSAGE!") );
    // } 
    // uta::assert_equal( 10, ado::messages.size() );

    dist -= obs;
    delete obs;

  }
};

REGISTER(ado_test) // this registers the test class