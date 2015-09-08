#ifndef LOGGING_SOURCE_HPP
#define LOGGING_SOURCE_HPP

#include <algorithm>
#include <chrono>
#include <thread>

#include <iostream>

#include "./message.hpp"
#include "./distributor.hpp"

#include "../thread/safe_circular_buffer.hpp"

namespace logging
{

  template<class Buffer>
  class Consumer
  {
    using value_type       =  typename Buffer::value_type;
    using distributor_type = ::logging::distributor;

    Buffer* m_container;
    // value_type m_item;

    public:
      Consumer(Buffer* buffer) 
      : m_container(buffer)
      {}

      void operator()()
      {
        static unsigned int loop_cnt = 0;
        // for (unsigned long i = 0L; i < total_elements; ++i)
        // {
        try
        {
          while(true)
          {
            ++loop_cnt;
            value_type item;
            // m_container->pop(&item);
            // distributor_type::instance().log( std::move( item ) );
            // distributor_type::instance().log( item );
          }
        } catch( std::exception& e )
        {
          std::cerr << "CONSUMER ERROR (" << loop_cnt << "): " << e.what() << std::endl;
        }

        std::cout << "async end! " << loop_cnt << std::endl;

        // }
      }
  };

  class source
  {
    public:
      using message_type     = ::logging::message;
      using distributor_type = ::logging::distributor;
      using container_type   = ::thread::safe_buffer_drop<message_type>;


      using size_type     = typename message_type::size_type;
      using level_type    = typename message_type::level_type;
      using channel_type  = typename message_type::channel_type;
      using string_type   = typename message_type::string_type;

      source( const std::string& name = "default", size_type fifo_size = 1000 )
      : m_channel(), m_container(fifo_size)
      {
        std::copy_n(name.begin(), name.size(), m_channel.begin());
        Consumer<container_type> consumer(&m_container);

        std::thread consume(consumer);
        consume.detach();
      }

      source(const source&)            = delete;
      source& operator=(const source&) = delete;

      void log( level_type lvl, const char* msg, size_type msg_len)
      {
        string_type str;
        std::copy_n(msg, msg_len, str.begin());
        log( lvl, str );
      }

      void log( level_type lvl, const string_type& msg )
      {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);

        m_container.push( message_type(m_channel, lvl, msg, in_time_t ) );

        // distributor_type::instance().log( std::move( message_type(m_channel, lvl, msg, in_time_t ) ) );
      }

      size_type container_size() const noexcept { return m_container.size(); }

    private:

      channel_type     m_channel;
      container_type   m_container;
  };

} // logging

#endif //LOGGING_SOURCE_HPP