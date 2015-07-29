#ifndef FACTORY_BLOG_DYNAMIC_OBSERVER_OBSERVER_HPP
#define FACTORY_BLOG_DYNAMIC_OBSERVER_OBSERVER_HPP

#include "./message.hpp"

namespace factory_blog
{
namespace dynamic_observer
{

  class observer
  {
    public:
      using message_type = ::factory_blog::dynamic_observer::message;
      using level_type   = typename message_type::level_type;

      observer( const level_type l  )
      : m_level(l)
      {}

      virtual ~observer() {}

      virtual void consume( const message_type& msg ) const = 0;

      level_type level() const noexcept { return m_level; }

      observer(const observer&)            = delete;
      observer& operator=(const observer&) = delete;

    private:
      const level_type  m_level;

  };
} // dynamic_observer
} // factory_blog

#endif // FACTORY_BLOG_DYNAMIC_OBSERVER_OBSERVER_HPP