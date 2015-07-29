#include <factory_blog/dynamic_observer/distributor.hpp>

#include <algorithm>
#include <functional>

namespace factory_blog
{
namespace dynamic_observer
{
  distributor::distributor() : m_observers() {}
  distributor::~distributor() { while(!m_observers.empty()) *this -= *(m_observers.begin()); }

  distributor& distributor::instance()
  {
    static distributor instance;
    return (instance);
  }

  const observer* distributor::get_observer(size_type idx) { return m_observers[idx]; }

  distributor& distributor::operator+=(observer* obs)
  {
    auto it = std::find( m_observers.begin(), m_observers.end(), obs );
    if( it == m_observers.end() ) m_observers.push_back(obs);
    return *this;
  }

  distributor& distributor::operator-=(const observer* obs)
  {
    auto it = std::find( m_observers.begin(), m_observers.end(), obs );
    if( it == m_observers.end() ) return *this;
    
    m_observers.erase(it);
    delete obs;
    return *this;
  }

  struct distributor::log_entry : public std::binary_function<observer *, message_type, bool> // bool is needed to make bind2nd happy
  {
    bool operator() (const observer * obs, const message_type & msg) const
    {
      if (obs->level() >= msg.level()) { obs->consume(msg); return (true); }
      return (false);
    }
  };

  void distributor::log(const message_type& msg)
  {   
    std::for_each(m_observers.begin(), m_observers.end(), std::bind2nd(log_entry(), msg)); 
  }


} // dynamic_observer
} // factory_blog