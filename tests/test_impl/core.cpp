#include "core.hpp"

#include <algorithm>

core::core() : m_sinks() {}
core::~core() { while(!m_sinks.empty()) remove_sink(*(m_sinks.begin())); }

bool core::remove_sink(const sink_ptr& s)
{
  auto it = std::find( m_sinks.begin(), m_sinks.end(), s );
  if( it == m_sinks.end() ) return false;
  
  m_sinks.erase(it);
  (*it)->stop();
  (*it).reset();

  return true;
}

void core::insert_sink(const sink_ptr& s )
{
  auto it = std::find( m_sinks.begin(), m_sinks.end(), s );
  if( it == m_sinks.end() ) m_sinks.push_back(s);
  s->start();
}

void core::transfer(const message_type& msg)
{   
  std::for_each(m_sinks.begin(), m_sinks.end(), [&](sink_ptr s){ s->receive(msg); });
}

void core::transfer(message_type&& msg)
{   
  std::for_each(m_sinks.begin(), m_sinks.end(), [&](sink_ptr s){ s->receive(std::move(msg)); });
}

core& core::instance()
{
  static core instance;
  return (instance);
}