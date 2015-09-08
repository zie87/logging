#ifndef LOGGING_CONSOLE_OBSERVER_HPP
#define LOGGING_CONSOLE_OBSERVER_HPP

#include "observer.hpp"
#include "message.hpp"

// #include <cstdio>
#include <iostream>

namespace logging
{
  class console_observer : public observer
  {
    public:
      using observer_type = ::logging::observer;
      using message_type  = typename observer_type::message_type;

      console_observer()  
      : observer_type()
      {}

      console_observer(const console_observer&)             = delete;
      console_observer& operator=(const console_observer&)  = delete;

    protected:
      virtual void write(const message_type& msg)
      {
        // std::printf("console_observer (%ld): %s\n", std::this_thread::get_id(), msg.get_string().c_str()); 
        std::clog << "[";
        for(auto& c : msg.get_channel() ) std::clog << c;
        std::clog << "] "
                  << "<" << static_cast<int>( msg.get_level() ) << "> "
                  << "(" << std::this_thread::get_id() << ") "
                  << ":";
        for(auto& c : msg.get_string() ) std::clog << c;
        std::clog << std::endl;

      }
  };

} // logging

#endif // LOGGING_CONSOLE_OBSERVER_HPP