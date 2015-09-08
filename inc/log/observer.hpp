#ifndef LOGGING_OBSERVER_HPP
#define LOGGING_OBSERVER_HPP

#include "message.hpp"

#include "../thread/safe_queue.hpp"
#include "../thread/semaphore.hpp"

#include <thread>
#include <future>
#include <condition_variable>

namespace logging
{
  class observer
  {
    public:
      using message_type = ::logging::message;

      observer();
      virtual ~observer();

      void start();
      void stop();
      void consume(const message_type& msg);

      observer(const observer&)             = delete;
      observer& operator=(const observer&)  = delete;

    protected:
      virtual void write(const message_type& msg) = 0;
      virtual void flush();

      virtual void process();
      virtual void run();

    private:
      bool proceed() const;

      using message_queue = ::thread::safe_queue<message_type>;

      message_queue               m_messages;
      ::thread::semaphore         m_semaphore;
      std::thread                 m_thread;
  
      bool                        m_proceed;
      bool                        m_running;
  };

} // logging

#endif // LOGGING_OBSERVER_HPP