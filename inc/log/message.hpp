#ifndef LOGGING_MESSAGE_HPP
#define LOGGING_MESSAGE_HPP

#include <array>
#include <algorithm>

namespace logging
{
  class message
  {
    public:
      enum class level {trace, debug, info, warn, error};
      
      using size_type    = unsigned long;
      using string_type  = std::array<char, 100>;
      using channel_type = std::array<char, 10>;
      using level_type   = level;
      using time_type    = std::intmax_t;
  
      message()
      : m_channel(), m_level(), m_msg(), m_time()
      {}

      message(channel_type channel, level_type lvl, const string_type& str, time_type time = 0 )
      : m_channel(channel), m_level(lvl), m_msg(str), m_time(time)
      {}

      message(const char* channel, size_type c_len, level_type lvl, const char* str, size_type s_len, time_type time = 0 )
      : m_channel(), m_level(lvl), m_msg(), m_time(time)
      {
        std::copy_n(channel, c_len, m_channel.begin());
        std::copy_n(str, s_len, m_msg.begin());
      }

      channel_type  get_channel() const noexcept { return m_channel; }
      string_type   get_string()  const noexcept { return m_msg; }
      level_type    get_level()   const noexcept { return m_level; }
      time_type     get_time()    const noexcept { return m_time; }
  
      message(const message& msg)
      : m_channel(msg.m_channel), m_level(msg.m_level), m_msg(msg.m_msg), m_time(msg.m_time)
      {}

      message& operator=(const message& msg)
      {
        m_channel = msg.m_channel;
        m_level   = msg.m_level;
        m_msg     = msg.m_msg;
        m_time    = msg.m_time;

        return *this;
      }

    private:
      channel_type  m_channel;
      level_type    m_level;
      string_type   m_msg;
      time_type     m_time;

  };

} // logging

#endif // LOGGING_MESSAGE_HPP