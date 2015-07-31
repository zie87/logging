#ifndef TEST_IMPL_CORE_HPP
#define TEST_IMPL_CORE_HPP

#include <vector>
#include <memory>

#include "message.hpp"
#include "sink.hpp"

class core
{
  public:
    // using observer_list = std::vector<observer*>;
    using message_type  = message;
    using sink_ptr      = std::shared_ptr<sink>;
    using sink_list     = std::vector<sink_ptr>;
    // using size_type     = unsigned long;

    static core& instance();
    ~core();

    // core& operator+=(observer* obs);
    // core& operator-=(const observer* obs);

    void transfer(const message_type& msg);
    // void log(message_type&& msg);

    bool remove_sink(const sink_ptr& sink);
    void insert_sink(const sink_ptr& sink);
    // size_type number_of_observers() const noexcept { return m_observers.size(); }
    // const observer* get_observer(size_type idx);

    core(const core&)            = delete;
    core& operator=(const core&) = delete;

  private:
    core();
    sink_list m_sinks;
};

#endif // TEST_IMPL_CORE_HPP