#ifndef LOGGING_THREAD_SAFE_CIRCULAR_BUFFER
#define LOGGING_THREAD_SAFE_CIRCULAR_BUFFER

#include <container/circular_buffer.hpp>

#include <mutex>
#include <condition_variable>
#include <functional>

namespace thread
{

  template<typename T>
  class safe_buffer_wait
  {

    public:
      using container_type = ::container::basic_circular_buffer<T>;

      using size_type  = typename container_type::size_type;
      using value_type = typename container_type::value_type;

      using reference         = typename container_type::reference;
      using const_reference   = typename container_type::const_reference;


      explicit safe_buffer_wait(size_type capa) 
      : m_unread(0), m_container(capa)
      , m_mutex(), m_not_full(), m_not_empty()
      {}

      safe_buffer_wait(const safe_buffer_wait&)            = delete;
      safe_buffer_wait& operator=(const safe_buffer_wait&) = delete;

      void push(value_type&& val)
      {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_not_full.wait(lock, std::bind(&safe_buffer_wait<value_type>::is_not_full, this));
        m_container.push( std::move(val) );
        ++m_unread;
        lock.unlock();
        m_not_empty.notify_one();
      }

      void pop(value_type* pItem)
      {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_not_empty.wait(lock, std::bind(&safe_buffer_wait<value_type>::is_not_empty, this));
        *pItem = m_container.front();
        --m_unread;
        m_container.pop();
        lock.unlock();
        m_not_full.notify_one();
      }

    protected:
      bool is_not_empty() const { return m_unread > 0; }
      bool is_not_full() const { return m_unread < m_container.capacity(); }

    private:
      size_type m_unread;
      container_type m_container;

      std::mutex              m_mutex;
      std::condition_variable m_not_full;
      std::condition_variable m_not_empty;
  };

  template<typename T>
  class safe_buffer_drop
  {

    public:
      using container_type = ::container::basic_circular_buffer<T>;

      using size_type  = typename container_type::size_type;
      using value_type = typename container_type::value_type;

      using reference         = typename container_type::reference;
      using const_reference   = typename container_type::const_reference;


      explicit safe_buffer_drop(size_type capa) 
      : m_unread(0), m_container(capa)
      , m_mutex()
      {}

      safe_buffer_drop(const safe_buffer_drop&)            = delete;
      safe_buffer_drop& operator=(const safe_buffer_drop&) = delete;

      void push(value_type&& val)
      {
        std::unique_lock<std::mutex> lock(m_mutex);
        // m_not_full.wait(lock, std::bind(&safe_buffer<value_type>::is_not_full, this));
        if( is_not_full() )
        {
          m_container.push( std::move(val) );
          ++m_unread;
        }
        lock.unlock();
        // m_not_empty.notify_one();
      }

      void pop(value_type* pItem)
      {
        std::unique_lock<std::mutex> lock(m_mutex);
        // m_not_empty.wait(lock, std::bind(&safe_buffer<value_type>::is_not_empty, this));
        if( is_not_empty() )
        { 
          *pItem = m_container.front();
          --m_unread;
          m_container.pop();
        }
        lock.unlock();
        // m_not_full.notify_one();
      }

      size_type size() const noexcept { return m_container.size(); }

    protected:
      bool is_not_empty() const { return m_unread > 0; }
      bool is_not_full() const { return m_unread < m_container.capacity(); }

    private:
      size_type m_unread;
      container_type m_container;

      std::mutex              m_mutex;
      // std::condition_variable m_not_full;
      // std::condition_variable m_not_empty;
  };

} //thread
#endif // LOGGING_THREAD_SAFE_CIRCULAR_BUFFER