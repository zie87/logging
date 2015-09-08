#include <log/observer.hpp> 

#include <chrono>


namespace logging
{
  observer::observer()
  : m_messages(), m_semaphore(), m_thread(), m_proceed(false), m_running(false)
  {}

  observer::~observer()
  {
    m_messages.clear();
  }

  void observer::start() 
  {
    m_proceed = true;
    m_thread = std::thread( &observer::run, this );
    m_thread.detach();
  }

  void observer::stop() 
  {  
    m_proceed = false; 
    while( m_running ) std::this_thread::sleep_for(std::chrono::milliseconds(2));  
    flush();
  }
  
  void observer::consume(const message_type& msg) 
  { 
    m_messages.push(msg); 
    m_semaphore.notify();
  }

  void observer::process()
  {
    m_semaphore.wait();
    write( m_messages.pop() );
  }

  void observer::run()
  {
    m_running = true;
    while ( proceed() ){ process(); std::this_thread::sleep_for(std::chrono::milliseconds(1)); }
    m_running = false;
  } 

  void observer::flush(){ while( !m_messages.empty() ) { write( m_messages.pop() ); } }

  bool observer::proceed() const
  {
    static std::mutex mtx;
    mtx.lock();
    bool con = m_proceed;
    mtx.unlock();
    return con;
  }

} // logging