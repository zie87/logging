#include <factory_blog/async_observer/console_observer.hpp>

#include <iostream>

namespace factory_blog
{
namespace async_observer
{
  int console_observer::m_instance_cnt = 0;

  console_observer::console_observer(const level_type l)
  : observer(l), m_instance_num(++m_instance_cnt)
  {}

  console_observer::~console_observer()
  {
    std::clog << "deleting console_observer: " << m_instance_num << std::endl;
  }

  void console_observer::consume( const message_type& msg ) const
  {
    std::clog << "console_observer #" << m_instance_num << ": "
              << "[" << msg.channel() << "] " 
              << msg.text() << std::endl;
  }

} // async_observer
} // factory_blog