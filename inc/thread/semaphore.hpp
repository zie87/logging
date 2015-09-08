#ifndef LOGGING_THREAD_SEMAPHORE_HPP
#define LOGGING_THREAD_SEMAPHORE_HPP

#include <mutex>
#include <condition_variable>

//http://stackoverflow.com/questions/4792449/c0x-has-no-semaphores-how-to-synchronize-threads

namespace thread
{

class semaphore
{
  public:
    using native_handle_type = std::condition_variable::native_handle_type;

    explicit semaphore(size_t n = 0);

    semaphore(const semaphore&)            = delete;
    semaphore& operator=(const semaphore&) = delete;

    void notify();
    void wait();

    template<class Rep, class Period>
    bool wait_for(const std::chrono::duration<Rep, Period>& d);
    template<class Clock, class Duration>
    bool wait_until(const std::chrono::time_point<Clock, Duration>& t);

    native_handle_type native_handle();

  private:
    size_t                  m_count;
    std::mutex              m_mutex;
    std::condition_variable m_condition;
};

inline semaphore::semaphore(size_t n)
: m_count{n}, m_mutex(), m_condition()
{}

inline void semaphore::notify()
{
  std::lock_guard<std::mutex> lock{m_mutex};
  ++m_count;
  m_condition.notify_one();
}

inline void semaphore::wait()
{
  std::unique_lock<std::mutex> lock{m_mutex};
  m_condition.wait(lock, [&]{ return m_count > 0; });
  --m_count;
}

template<class Rep, class Period>
bool semaphore::wait_for(const std::chrono::duration<Rep, Period>& d)
{
  std::unique_lock<std::mutex> lock{m_mutex};
  auto finished = m_condition.wait_for(lock, d, [&]{ return m_count > 0; });
  if (finished)  --m_count;
  return finished;
}

template<class Clock, class Duration>
bool semaphore::wait_until(const std::chrono::time_point<Clock, Duration>& t)
{
  std::unique_lock<std::mutex> lock{m_mutex};
  auto finished = m_condition.wait_until(lock, t, [&]{ return m_count > 0; });
  if (finished) --m_count;
  return finished;
}

inline semaphore::native_handle_type semaphore::native_handle() { return m_condition.native_handle(); }


} // thread


#endif // LOGGING_THREAD_SEMAPHORE_HPP