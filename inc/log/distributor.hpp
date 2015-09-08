#ifndef LOGGING_DISTRIBUTOR_HPP
#define LOGGING_DISTRIBUTOR_HPP

#include "observer.hpp"
#include "message.hpp"
#include "../thread/safe_queue.hpp"
#include "../thread/semaphore.hpp"

#include <thread>
#include <future>
#include <condition_variable>

#include <vector>
#include <mutex>

namespace logging
{

  class distributor
  {
    public:
      using observer_type = ::logging::observer;
      using message_type  = ::logging::message;

      static distributor& instance();
      ~distributor();

      void append_observer(observer_type* obs);
      void remove_observer(const observer_type* obs);

      void log(const message_type& msg);

      distributor(const distributor&)            = delete;
      distributor& operator=(const distributor&) = delete;

    private:
      void start();
      void stop();
      void process();
      void run();
      bool proceed() const;

      distributor();

      std::vector<observer_type*> m_observers;
      std::mutex                  m_mutex;

      using message_queue = ::thread::safe_queue<message_type>;

      message_queue               m_messages;
      ::thread::semaphore         m_semaphore;
      std::thread                 m_thread;

      bool                        m_proceed;
      bool                        m_running;
  };

} // logging

#endif // LOGGING_DISTRIBUTOR_HPP