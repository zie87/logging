#ifndef FACTORY_BLOG_ASYNC_OBSERVER_DISTRIBUTOR_HPP
#define FACTORY_BLOG_ASYNC_OBSERVER_DISTRIBUTOR_HPP

#include "./message.hpp"
#include "./observer.hpp"

#include <vector>

namespace factory_blog
{
namespace async_observer
{
  class distributor
  {
    public:
      using observer_list = std::vector<observer*>;
      using message_type  = ::factory_blog::async_observer::message;
      using size_type     = unsigned long;

      static distributor& instance();
      ~distributor();

      distributor& operator+=(observer* obs);
      distributor& operator-=(const observer* obs);

      void log(const message_type& msg);
      void log(message_type&& msg);

      size_type number_of_observers() const noexcept { return m_observers.size(); }
      const observer* get_observer(size_type idx);

      distributor(const distributor&)            = delete;
      distributor& operator=(const distributor&) = delete;

    private:
      distributor();
      observer_list m_observers;

  };
} // async_observer
} // factory_blog

#endif // FACTORY_BLOG_ASYNC_OBSERVER_DISTRIBUTOR_HPP