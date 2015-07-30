#ifndef FACTORY_BLOG_ASYNC_OBSERVER_MESSAGE_QUEUE_HPP
#define FACTORY_BLOG_ASYNC_OBSERVER_MESSAGE_QUEUE_HPP

#include "./message.hpp"

#include <queue>
#include <mutex>

namespace factory_blog
{
namespace async_observer
{

  class message_queue
  {
    public:
      using message_type   = ::factory_blog::async_observer::message;
      using container_type = std::queue<message_type>; 
      using mutex_type     = std::recursive_mutex;

      message_queue();
      ~message_queue();

      message_queue& operator+=(const message_type& msg);

      void push(const message_type& msg);
      void push( message_type&& msg );
      message_type pop();

      message_queue(const message_queue&)            = delete;
      message_queue& operator=(const message_queue&) = delete;

    private:
      container_type m_container;
      mutex_type     m_mutex;

  };
} // async_observer
} // factory_blog

#endif // FACTORY_BLOG_ASYNC_OBSERVER_MESSAGE_QUEUE_HPP