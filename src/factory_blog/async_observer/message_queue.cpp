#include <factory_blog/async_observer/message_queue.hpp>

#include <stdexcept>

namespace factory_blog
{
namespace async_observer
{

  message_queue::message_queue()
  :m_container(), m_mutex()
  {}

  message_queue::~message_queue() {}

  message_queue& message_queue::operator+=(const message_type& msg) 
  {  
    push(msg);
    return *this;
  }

  void message_queue::push(const message_type& msg)
  {
    std::lock_guard<mutex_type> lock(m_mutex);
    m_container.push(msg);
  }

  void message_queue::push( message_type&& msg )
  {
    std::lock_guard<mutex_type> lock(m_mutex);
    m_container.push(std::move(msg));
  }

  typename message_queue::message_type message_queue::pop()
  {
    std::lock_guard<mutex_type> lock(m_mutex);
    if( m_container.empty() ) throw std::underflow_error("ERROR: pop on empty message_queue!");

    auto out_msg = m_container.front();
    m_container.pop();

    return out_msg;
  }
 
} // async_observer
} // factory_blog