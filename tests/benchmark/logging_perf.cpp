
#include <log/message.hpp>
#include <log/distributor.hpp>
#include <log/console_observer.hpp>

#include <thread>         
#include <chrono>        
#include <vector> 
#include <algorithm>  
#include <iostream>
#include <iomanip>

// enum class level { trace = 0, debug, info, warning, error };

using level = ::logging::message::level;

template< typename CharT, typename TraitsT >
inline std::basic_ostream< CharT, TraitsT >& operator<<(std::basic_ostream< CharT, TraitsT >& strm, level l)
{
  static const char* const str[] = { "trace","debug", "info", "warn", "error" };

  auto lvl = static_cast< int >(l);
  if (static_cast< std::size_t >(lvl) < (sizeof(str) / sizeof(*str))) strm << str[lvl];
  else strm << lvl;

  return strm;
}

using message_type     = logging::message;
using distributor_type = logging::distributor;
using observer_type    = logging::console_observer;


#include "timer.hpp"


static std::vector<std::size_t> time_vector;

void send_messages()
{
  benchmark::steady_timer t(time_vector);

  char chan[1] = { char(65) };

  message_type t_msg( chan, 1, level::trace,   "This is a trace-log!"  , 21  );
  message_type d_msg( chan, 1, level::debug,   "This is a debug-log!"  , 21  );
  message_type i_msg( chan, 1, level::info,    "This is an info-log!"  , 21  );
  message_type w_msg( chan, 1, level::warn,    "This is a warning-log!", 23  );
  message_type e_msg( chan, 1, level::error,   "This is an error-log!" , 22  );
  distributor_type::instance().log(t_msg);
  distributor_type::instance().log(d_msg);
  distributor_type::instance().log(i_msg);
  distributor_type::instance().log(w_msg);
  distributor_type::instance().log(e_msg);
}

void send_messages_mt_impl(unsigned int id)
{
  char chan[1] = { char(65 + id) };

  message_type t_msg( chan, 1, level::trace,   "This is a trace-log!"  , 21  );
  message_type d_msg( chan, 1, level::debug,   "This is a debug-log!"  , 21  );
  message_type i_msg( chan, 1, level::info,    "This is an info-log!"  , 21  );
  message_type w_msg( chan, 1, level::warn,    "This is a warning-log!", 23  );
  message_type e_msg( chan, 1, level::error,   "This is an error-log!" , 22  );
  distributor_type::instance().log(t_msg);
  distributor_type::instance().log(d_msg);
  distributor_type::instance().log(i_msg);
  distributor_type::instance().log(w_msg);
  distributor_type::instance().log(e_msg);
}

void send_messages_mt()
{
  std::vector<std::thread> threads;
  benchmark::steady_timer t(time_vector);
  for(unsigned int i = 1; i < 11; ++i){ threads.push_back(std::thread(send_messages_mt_impl, i));}
  std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));
}

void print_time_vector( const std::string& name )
{
  if( time_vector.empty() ) return;
  std::size_t max = 0;
  std::size_t min = time_vector[0];
  std::size_t sum = 0;
  std::size_t avg = 0;

  for(unsigned int i = 0; i < time_vector.size(); ++ i)
  {
    auto current = time_vector[i];
    if( current < min ) min = current;
    if( current > max ) max = current;
    sum += current;
  }
  if( time_vector.size() > 2 )
  {
    sum -= (min + max); // remove anomalies
    avg = sum / ( time_vector.size() - 2 );
  } else
  {
    avg = sum / time_vector.size();
  }

  std::cout << std::endl;
  std::cout << name << std::setw(30 - name.size() ) << "min time:\t" << std::right << std::setw(10) << min << " ms" << std::endl;
  std::cout << name << std::setw(30 - name.size() ) << "max time:\t" << std::right << std::setw(10) << max << " ms" << std::endl;
  std::cout << name << std::setw(30 - name.size() ) << "avg time:\t" << std::right << std::setw(10) << avg << " ms" << std::endl;
  std::cout << std::endl;
}


int main()
{

  static const unsigned int repeations = 10;
  distributor_type& inst = distributor_type::instance();
  observer_type* obs1 = new observer_type();
  inst.append_observer(obs1);

  time_vector.clear();
  for( unsigned int i=0; i < repeations; ++i  ) { send_messages();}
  print_time_vector("send_messages");


  time_vector.clear();
  for( unsigned int i=0; i < repeations; ++i  ) { send_messages_mt(); }
  print_time_vector("send_messages_mt");


  inst.remove_observer(obs1);
  delete obs1;

  return 0;
}