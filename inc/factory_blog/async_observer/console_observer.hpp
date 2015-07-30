#ifndef FACTORY_BLOG_ASYNC_OBSERVER_CONSOLE_OBSERVER_HPP
#define FACTORY_BLOG_ASYNC_OBSERVER_CONSOLE_OBSERVER_HPP

#include "./observer.hpp"

namespace factory_blog
{
namespace async_observer
{

  class console_observer : public observer
  {
    public:
      using message_type = typename observer::message_type;
      using level_type   = typename message_type::level_type;

      console_observer(const level_type l);
      ~console_observer();

      console_observer(const console_observer&)            = delete;
      console_observer& operator=(const console_observer&) = delete;

    protected:
      
      virtual void consume( const message_type& msg ) const override;

    private:
      const  int m_instance_num;
      static int m_instance_cnt; 
  };
} // async_observer
} // factory_blog

#endif // FACTORY_BLOG_ASYNC_OBSERVER_CONSOLE_OBSERVER_HPP