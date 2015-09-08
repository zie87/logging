#include <libunittest/all.hpp>

namespace ut  = unittest;
namespace uta = ut::assertions;

#include <container/circular_buffer.hpp>

struct test_circular_buffer : ut::testcase<> 
{

  static void run()
  {
      UNITTEST_CLASS(test_circular_buffer);
      UNITTEST_RUN(basic_test);
      UNITTEST_RUN(exception_test);
  }

  test_circular_buffer() {} // executed before each test and before set_up()
  ~test_circular_buffer() {} // executed after each test and after tear_down()

  void set_up() {} // executed before each test and after constructor
  void tear_down() {} // executed after each test and before destructor

  void basic_test()
  {
    container::basic_circular_buffer<char> cb(10);

    uta::assert_equal( 10, cb.capacity() );
    uta::assert_equal(  0, cb.size() );
    uta::assert_true( cb.empty() );

    char test_data[10] = {'a','b','c','d','e','f','g','h','i','j'};

    for(unsigned int i = 0; i < 10; ++i) cb.push( test_data[i] );
    uta::assert_equal( 10, cb.size() );
    uta::assert_false( cb.empty() );

    for(unsigned int i = 0; i < 10; ++i)
    {
      uta::assert_equal( test_data[i], cb.front() );
      uta::assert_equal( test_data[9], cb.back() );
      cb.pop();
    }

    uta::assert_equal( 10, cb.capacity() );
    uta::assert_equal(  0, cb.size() );
    uta::assert_true( cb.empty() );


    for(unsigned int i = 0; i < 10; ++i)
    {
      char val = test_data[i];
      cb.push( std::move(val) );

    } 
    uta::assert_equal( 10, cb.size() );
    uta::assert_false( cb.empty() );
    for(unsigned int i = 0; i < 10; ++i)
    {
      uta::assert_equal( test_data[i], cb.front() );
      uta::assert_equal( test_data[9], cb.back() );
      cb.pop();
    }
  }

  void exception_test()
  {
    container::basic_circular_buffer<char> cb(0);
    try 
    { 
      cb.pop(); 
      uta::assert_true(false);
    } catch( std::out_of_range& e )
    {
      uta::assert_true(true);
    } catch(...)
    {
      uta::assert_true(false);
    }

    try 
    { 
      cb.front(); 
      uta::assert_true(false);
    } catch( std::out_of_range& e )
    {
      uta::assert_true(true);
    } catch(...)
    {
      uta::assert_true(false);
    }

    try 
    { 
      cb.back(); 
      uta::assert_true(false);
    } catch( std::out_of_range& e )
    {
      uta::assert_true(true);
    } catch(...)
    {
      uta::assert_true(false);
    }

    try 
    { 
      cb.push('A'); 
      uta::assert_true(false);
    } catch( std::out_of_range& e )
    {
      uta::assert_true(true);
    } catch(...)
    {
      uta::assert_true(false);
    }
  }




};

REGISTER(test_circular_buffer) // this registers the test class