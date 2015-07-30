#include <factory_blog/async_observer/distributor.hpp>

#include <algorithm>
#include <functional>

namespace factory_blog
{
namespace async_observer
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
    obs->start();
    return *this;
  }

  distributor& distributor::operator-=(const observer* obs)
  {
    auto it = std::find( m_observers.begin(), m_observers.end(), obs );
    if( it == m_observers.end() ) return *this;
    
    m_observers.erase(it);
    (*it)->receive( message_type( "distributor", message_type::level_type::event, "", message_type::internal_type::end ) );
    (*it)->stop();
    // WAIT?
    // delete obs;
    return *this;
  }

  void distributor::log(const message_type& msg)
  {   
    std::for_each(m_observers.begin(), m_observers.end(), [&](observer* obs){ if (obs->level() >= msg.level()) { obs->receive(msg);} });
  }

  void distributor::log(message_type&& msg)
  {   
    std::for_each(m_observers.begin(), m_observers.end(), [&](observer* obs){ if (obs->level() >= msg.level()) { obs->receive(std::move(msg));} });
  }

} // async_observer
} // factory_blog