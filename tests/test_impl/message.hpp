#ifndef TEST_IMPL_MESSAGE_HPP
#define TEST_IMPL_MESSAGE_HPP


#include "fixed_string.hpp"

#include <iostream>

struct message
{
  enum class severity_level {trace = 0, debug, info, warning, error, critical };

  using channel_type = fixed_string_buffer<32>;
  using string_type  = fixed_string_buffer<128>;

  message(const string_type& i = 0)
  : item(i), channel("NO_CHANNEL"), level(severity_level::info)
  {}

  message(channel_type chan, const string_type& i, severity_level l = severity_level::info)
  : item(i), channel(chan), level(l)
  {}

  message(const message& msg)
  : item( msg.item ), channel( msg.channel ), level(msg.level)
  {}

  message(message&& msg)
  : item( std::move(msg.item) ), channel( std::move(msg.channel) ), level( std::move(msg.level) )
  {}

  message& operator=(const message& msg)
  {
    message tmp(msg);
    swap(tmp);
    return *this;
  }

  message& operator=(message&& msg)
  {
    move(std::move(msg));
    return *this;
  }

  void move(message&& msg)
  {
    item    = std::move( msg.item );
    channel = std::move( msg.channel );
    level   = std::move( msg.level );
  }

  void swap(message& msg)
  {
    std::swap( item, msg.item );
    std::swap( channel, msg.channel );
    std::swap( level, msg.level );
  }

  string_type       item;
  channel_type      channel; 
  severity_level    level;
};

// The formatting logic for the severity level
template< typename CharT, typename TraitsT >
inline std::basic_ostream< CharT, TraitsT >& operator<<(std::basic_ostream< CharT, TraitsT >& out, message::severity_level l)
{
  static const char* const str[] = {"trace", "debug","info","warn","error","critical" };

  int lvl = static_cast<int>(l);
  if (static_cast< std::size_t >(lvl) < (sizeof(str) / sizeof(*str))) out << str[lvl];
  else out << lvl;

  return out;
}

#endif // TEST_IMPL_MESSAGE_HPP