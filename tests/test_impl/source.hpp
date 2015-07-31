#ifndef TEST_IMPL_SOURCE_HPP
#define TEST_IMPL_SOURCE_HPP

#include <mutex>
#include <thread>
#include <future>
#include <atomic>
#include <condition_variable>

#include <queues/mpmc_buffer.hpp>

#include "sink.hpp"
#include "core.hpp"
#include "message.hpp"

class source
{
  public:
    using value_type      = message;
    using container_type  = ::mpmc_buffer<value_type>;
    using size_type       = typename container_type::size_type;
    using mutex_type      = std::recursive_mutex;

    explicit source(size_type size = 65536)
    : m_container(size), m_mutex()
    , m_stop_process(true), m_process_future()
    {}

    virtual ~source()
    {
      stop();
      value_type item;
      while( m_container.pop( item ) ){;}
    }

    source(const source&)            = delete;
    source& operator=(const source&) = delete;

    void flush()
    {
      std::unique_lock<mutex_type> lock(m_mutex);
      value_type item;
      while( m_container.pop(item) ){ consume(item); }
      lock.unlock();
    }

    void log(const value_type& val)
    {
      // std::unique_lock<mutex_type> lock(m_mutex);
      start();
      while(! m_container.push(val) ) {;}
      // lock.unlock();
    }

    // dummy!!!!
    void log(const int& i) { log( value_type(i) ); }

    void start()
    {
      if( m_stop_process.load(std::memory_order_acquire) )
      {
        m_stop_process.store(false, std::memory_order_release);
        m_process_future = std::move( std::async(std::launch::async, &source::process, this) );
      }
    }

    void stop()
    {
      if( !m_stop_process.load(std::memory_order_acquire) )
      {  
        m_stop_process.store(true, std::memory_order_release);
        if(m_process_future.valid())
        {
          m_process_future.get();
        }
      }
    }

    void process()
    {
      static std::mutex mtx;
      while( !m_stop_process.load(std::memory_order_acquire)  )
      {
        std::unique_lock<std::mutex> lock(mtx);
        value_type val; 
        if(m_container.pop(val))
        {
          consume(val);
        }
        lock.unlock();
      }
      m_stop_process.store(true, std::memory_order_release);
    }


  private:
    void consume(const value_type& val) { core::instance().transfer(val); }

    container_type          m_container;
    mutex_type              m_mutex;

    std::atomic<bool>       m_stop_process;
    std::future<void>       m_process_future;
};

#endif // TEST_IMPL_SOURCE_HPP