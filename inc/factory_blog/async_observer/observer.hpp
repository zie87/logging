#ifndef FACTORY_BLOG_ASYNC_OBSERVER_OBSERVER_HPP
#define FACTORY_BLOG_ASYNC_OBSERVER_OBSERVER_HPP

#include "./message_queue.hpp"

#include <mutex>
#include <atomic>
#include <condition_variable>

namespace factory_blog
{
namespace async_observer
{

  class observer
  {
    public:
      using container_type = message_queue;
      using message_type   = typename container_type::message_type;
      using level_type     = typename message_type::level_type;

      using size_type      = unsigned long;
      using mutex_type     = std::recursive_mutex;

      virtual ~observer();

      void receive(const message_type& msg);
      void receive(message_type&& msg);

      level_type level() const noexcept { return m_level; }

      observer(const observer&)            = delete;
      observer& operator=(const observer&) = delete;

      void start();
      void stop();

    protected:
      observer( const level_type l  );
      virtual void consume( const message_type& msg ) const = 0;

      virtual void process();

      bool is_not_empty() const { return m_unread > 0; }

    private:
      const level_type    m_level;
      container_type      m_container;

    protected:
      mutex_type              m_mutex;

      std::atomic<bool>       m_stop_process;
      std::atomic<bool>       m_stopped;

      std::condition_variable m_not_empty;
      std::condition_variable m_stopped_condition;
      size_type               m_unread;

  };

} // async_observer
} // factory_blog

#endif // FACTORY_BLOG_ASYNC_OBSERVER_OBSERVER_HPP