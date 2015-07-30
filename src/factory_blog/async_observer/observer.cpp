#include <factory_blog/async_observer/observer.hpp>

#include <stdexcept>
#include <thread>
#include <iostream>

namespace factory_blog
{
namespace async_observer
{

  observer::observer( const level_type l  )
  : m_level(l), m_container()
  , m_mutex()
  , m_stop_process(true), m_stopped(false)
  , m_not_empty(), m_stopped_condition()
  , m_unread(0)
  {}
 
  observer::~observer()
  {
    stop();
    while( m_unread )
    {
      m_container.pop();
      --m_unread;
    }
  }

  void observer::start()
  {
    // bool s = m_stop_process.load();
    if( m_stop_process )
    {
      m_stop_process.store(false);
      std::thread run( &observer::process, this );
      run.detach();
    }
  }

  void observer::stop()
  {
    if( !m_stop_process )
    {  
      std::mutex mtx;
      std::unique_lock<std::mutex> lock(mtx);
      m_stop_process.store(true);
      m_stopped_condition.wait( lock );
    }
  }

  void observer::receive(const message_type& msg)
  {
    std::unique_lock<mutex_type> lock(m_mutex);
    m_container.push(msg);
    ++m_unread;
    lock.unlock();
    m_not_empty.notify_one();
  }
  void observer::receive(message_type&& msg)
  {
    std::unique_lock<mutex_type> lock(m_mutex);
    m_container.push(std::move(msg));
    ++m_unread;
    lock.unlock();
    m_not_empty.notify_one();
  }


  void observer::process()
  {
    static std::mutex mtx;
    while( !m_stop_process )
    {
      std::unique_lock<std::mutex> lock(mtx);
      m_not_empty.wait(lock, std::bind(&observer::is_not_empty, this));
      auto msg = m_container.pop();
      --m_unread;
      if( msg.internal() == message_type::internal_type::end )
      {
        m_stop_process.store(true);
      } else { consume(msg);}
      lock.unlock();
    }
    m_stopped_condition.notify_one();
  }


} // async_observer
} // factory_blog