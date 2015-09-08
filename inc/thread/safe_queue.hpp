#ifndef LOGGING_THREAD_SAFE_QUEUE_HPP
#define LOGGING_THREAD_SAFE_QUEUE_HPP

#include <queue>
#include <mutex>
#include <condition_variable>

// http://stackoverflow.com/questions/15278343/c11-thread-safe-queue

namespace thread
{

  template <class T>
  class safe_queue
  {
    public:
      using value_type = T;
      using queue_type = std::queue<value_type>;
      using size_type  = typename queue_type::size_type;

      safe_queue()
      : m_queue(), m_mutex(), m_condition()
      {}
      ~safe_queue(){}

      safe_queue(const safe_queue&)            = delete;
      safe_queue& operator=(const safe_queue&) = delete;

      size_type size() const noexcept
      {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.size();
      }

      bool empty() const noexcept
      {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.empty();
      }

      void push(value_type v)
      {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push(v);
        m_condition.notify_one();
      }

      value_type pop()
      {
        std::unique_lock<std::mutex> lock(m_mutex);
        while(m_queue.empty()) { m_condition.wait(lock); }
        value_type val = m_queue.front();
        m_queue.pop();
        return val;
      }

      void clear() noexcept
      {
        std::unique_lock<std::mutex> lock(m_mutex);
        while( !m_queue.empty() ) m_queue.pop();
      }

    private:
      queue_type              m_queue;
      mutable std::mutex      m_mutex;
      std::condition_variable m_condition;
  };

} // thread


#endif // LOGGING_THREAD_SAFE_QUEUE_HPP