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

  void distributor::log(const message_type& msg)
  {   
    std::for_each(m_observers.begin(), m_observers.end(), [&](const observer* obs){ if (obs->level() >= msg.level()) { obs->consume(msg);} });
  }


} // dynamic_observer
} // factory_blog