#ifndef FACTORY_BLOG_DYNAMIC_OBSERVER_MESSAGE_HPP
#define FACTORY_BLOG_DYNAMIC_OBSERVER_MESSAGE_HPP

#include <string>

namespace factory_blog
{
namespace dynamic_observer
{

  class message
  {
    public:
      enum class level { error = 1, event, trace, debug };

      using string_type  = std::string;
      using level_type   = level;
      using channel_type = std::string;

      message( const channel_type a, const level_type l, char* s )
      : m_text(s), m_channel(a), m_level(l)
      {}

      message( const message& msg )
      : m_text( msg.m_text ), m_channel(msg.m_channel), m_level(msg.m_level)
      {}

      string_type  text()    const noexcept { return m_text; }
      channel_type channel() const noexcept { return m_channel; }
      level_type   level()   const noexcept { return m_level; }

      message& operator=(const message&) = delete;

    private:
      const string_type   m_text;
      const channel_type  m_channel;
      const level_type    m_level;

  };
} // dynamic_observer
} // factory_blog

#endif // FACTORY_BLOG_DYNAMIC_OBSERVER_MESSAGE_HPP