#ifndef TEST_IMPL_SINK_HPP
#define TEST_IMPL_SINK_HPP

#include <mutex>
#include <thread>
#include <future>
#include <atomic>
#include <condition_variable>

#include <queues/spsc_buffer.hpp>

#include "message.hpp"

class sink
{
  public:
    using value_type      = message;
    using container_type  = ::spsc_buffer<value_type>;
    using size_type       = typename container_type::size_type;

    using mutex_type      = std::recursive_mutex;

    explicit sink(size_type size = 65536)
    : m_container(size), m_mutex()
    , m_stop_process(true), m_process_future()
    {}

    virtual ~sink()
    {
      stop();
      value_type item;
      while( m_container.pop( item ) ){;}
    }

    sink(const sink&)            = delete;
    sink& operator=(const sink&) = delete;

    void flush()
    {
      std::unique_lock<mutex_type> lock(m_mutex);
      value_type item;
      while( m_container.pop(item) ){ consume(item); }
      lock.unlock();
    }

    void receive(const value_type& val)
    {
      std::unique_lock<mutex_type> lock(m_mutex);
      m_container.push(val);
      lock.unlock();
    }

    void receive(value_type&& val)
    {
      std::unique_lock<mutex_type> lock(m_mutex);
      m_container.push( std::move(val) );
      lock.unlock();
    }

    void start()
    {
      if( m_stop_process.load(std::memory_order_acquire) )
      {
        m_stop_process.store(false, std::memory_order_release);
        m_process_future = std::move( std::async(std::launch::async, &sink::process, this) );
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


  protected:
    virtual void consume(const value_type& val) = 0;

  private:
    container_type          m_container;
    mutex_type              m_mutex;

    std::atomic<bool>       m_stop_process;
    std::future<void>       m_process_future;
};

#endif // TEST_IMPL_SINK_HPP