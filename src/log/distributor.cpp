
#include <log/distributor.hpp>

#include <algorithm>

#include <iostream> 

namespace logging
{
  distributor& distributor::instance()
  {
    static distributor inst;
    inst.start();
    return inst;
  }

  distributor::distributor()
  : m_observers(), m_mutex(), m_messages(), m_semaphore(), m_thread(), m_proceed(false), m_running(false)
  {}

  distributor::~distributor()
  {
    for(auto && obs : m_observers) remove_observer(obs);
    m_messages.clear();
  }

  void distributor::append_observer(observer_type* obs)
  {
    m_mutex.lock();
    bool start_needed = false;
    auto find_it = std::find( m_observers.begin(), m_observers.end(), obs);
    if( find_it == m_observers.end() )
    {
      m_observers.push_back(obs);
      start_needed = true;
    } 
    m_mutex.unlock();

    if(start_needed) obs->start();
  }

  void distributor::remove_observer(const observer_type* obs)
  {
    m_mutex.lock();
    observer_type* stop_observer = nullptr;
    auto find_it = std::find( m_observers.begin(), m_observers.end(), obs);
    if( find_it != m_observers.end() )
    {
      stop_observer = *find_it;
      m_observers.erase(find_it);
    } 
    m_mutex.unlock();

    if(stop_observer) stop_observer->stop();
  }

  void distributor::process()
  {
    m_semaphore.wait();
    message_type msg = m_messages.pop();
    for(auto&& obs : m_observers) obs->consume(msg);
  }

  void distributor::run()
  {
    m_running = true;
    while ( proceed() ){ process(); std::this_thread::sleep_for(std::chrono::milliseconds(1)); }
    m_running = false;
  } 

  bool distributor::proceed() const
  {
    static std::mutex mtx;
    mtx.lock();
    bool con = m_proceed;
    mtx.unlock();
    return con;
  }

  void distributor::start() 
  {
    // std::lock_guard<std::mutex> lock(m_mutex);
    // if( !m_proceed )
    // {
    std::cout << "start in!" << std::endl;
    m_proceed = true;
    m_thread = std::thread( &distributor::run, this );
    m_thread.detach();
    std::cout << "start out!" << std::endl;
    // }
  }

  void distributor::stop() 
  {  
    m_proceed = false; 
    while( m_running ) std::this_thread::sleep_for(std::chrono::milliseconds(2));  
  }

  void distributor::log(const message_type& msg)
  {
    // static std::mutex mtx;
    // std::unique_lock<std::mutex> lock(mtx);
    try
    {
      m_messages.push(msg); 
      m_semaphore.notify();   
    } catch(std::exception& e)
    {
      std::cerr << "DISTRIBUTOR ERROR! " << e.what() << std::endl;
    }
  }


} // logging