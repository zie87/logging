#ifndef FACTORY_BLOG_ASYNC_OBSERVER_MESSAGE_HPP
#define FACTORY_BLOG_ASYNC_OBSERVER_MESSAGE_HPP

#include <string>

namespace factory_blog
{
namespace async_observer
{

  class message
  {
    public:
      enum class level    { error = 1, event, trace, debug };
      enum class internal { no_internal = 1, end };

      using string_type   = std::string;
      using level_type    = level;
      using channel_type  = std::string;
      using internal_type = internal;

      message( const channel_type a, const level_type l, const string_type& s, const internal_type i = internal_type::no_internal )
      : m_text(s), m_channel(a), m_level(l), m_internal(i)
      {}

      message( const message& msg )
      : m_text( msg.m_text ), m_channel(msg.m_channel), m_level(msg.m_level), m_internal(msg.m_internal)
      {}

      string_type   text()     const noexcept { return m_text; }
      channel_type  channel()  const noexcept { return m_channel; }
      level_type    level()    const noexcept { return m_level; }
      internal_type internal() const noexcept { return m_internal; }

      message& operator=(message msg)
      {
        swap(msg);
        return *this;
      }

      void swap( message& msg )
      {
        std::swap( m_text    , msg.m_text );
        std::swap( m_channel , msg.m_channel );
        std::swap( m_level   , msg.m_level );
        std::swap( m_internal, msg.m_internal);
      }


    private:
      string_type   m_text;
      channel_type  m_channel;
      level_type    m_level;
      internal_type m_internal;

  };
} // async_observer
} // factory_blog

#endif // FACTORY_BLOG_ASYNC_OBSERVER_MESSAGE_HPP