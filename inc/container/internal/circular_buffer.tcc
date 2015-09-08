#ifndef LOGGING_CONTAINER_INTERNAL_CIRCULAR_BUFFER_HPP
#define LOGGING_CONTAINER_INTERNAL_CIRCULAR_BUFFER_HPP

#include <algorithm>
#include <stdexcept>

namespace container
{
  template<typename T>
  basic_circular_buffer<T>::basic_circular_buffer(size_type capa)
  : m_capacity(capa), m_size(0), m_first_pos(0), m_last_pos(0), m_data(allocate(capa))
  { }

  template<typename T>
  basic_circular_buffer<T>::~basic_circular_buffer() 
  { 
    clean();
    if(m_data) deallocate(m_data); 
  }

  template<typename T>
  typename basic_circular_buffer<T>::pointer basic_circular_buffer<T>::allocate(size_type s) const
  { return(static_cast<pointer>(operator new(sizeof(value_type)*s))); }

  template<typename T>
  void basic_circular_buffer<T>::deallocate(pointer ptr) const
  { operator delete(ptr); }

  template<typename T>
  void basic_circular_buffer<T>::construct(pointer adr, const value_type& val) { new (adr) value_type(val); }
  
  template<typename T>
  void basic_circular_buffer<T>::construct(pointer adr, value_type&& val) { new (adr) value_type(std::move(val)); }

  template<typename T>
  void basic_circular_buffer<T>::destroy(pointer adr) { adr->~value_type();}

  template<typename T>
  void basic_circular_buffer<T>::clean()
  {
    while(m_size--) 
    {
      destroy(&m_data[m_first_pos++]);
      m_first_pos%=m_capacity;
    }
    m_first_pos = m_last_pos = 0;
  }

  template<typename T>
  void basic_circular_buffer<T>::push(const value_type& val)
  {
    if( ( capacity() - size() ) == 0 ) throw std::out_of_range("ERROR: push on full buffer!");  
    construct(&m_data[m_last_pos++], val);
    m_last_pos%=m_capacity;
    ++m_size;
  }
  
  template<typename T>
  void basic_circular_buffer<T>::push( value_type&& val )
  {
    if( ( capacity() - size() ) == 0 ) throw std::out_of_range("ERROR: push on full buffer!");  
    construct(&m_data[m_last_pos++], std::move(val));
    m_last_pos%=m_capacity;
    ++m_size;
  }

  template<typename T>
  void basic_circular_buffer<T>::pop() 
  {
    if( empty() ) throw std::out_of_range("ERROR: pop on empty buffer!");
    destroy(&m_data[m_first_pos++]);
    m_first_pos%=m_capacity;
    --m_size;
  }

  template<typename T>
  typename basic_circular_buffer<T>::reference basic_circular_buffer<T>::front() 
  {
    if( empty() ) throw std::out_of_range("ERROR: front on empty buffer!");
    return(m_data[m_first_pos]);
  }

  template<typename T>
  typename basic_circular_buffer<T>::const_reference basic_circular_buffer<T>::front() const
  {
    if( empty() ) throw std::out_of_range("ERROR: front on empty buffer!");
    return(m_data[m_first_pos]);
  }

  template<typename T>
  typename basic_circular_buffer<T>::reference basic_circular_buffer<T>::back() 
  {
    if( empty() ) throw std::out_of_range("ERROR: back on empty buffer!");
    size_type pos = (m_last_pos+m_capacity-1)%m_capacity;
    return(m_data[pos]);
  }

  template<typename T>
  typename basic_circular_buffer<T>::const_reference basic_circular_buffer<T>::back() const
  {
    if( empty() ) throw std::out_of_range("ERROR: back on empty buffer!");
    size_type pos = (m_last_pos+m_capacity-1)%m_capacity;
    return(m_data[pos]);
  }

} // container

#endif // LOGGING_CONTAINER_INTERNAL_CIRCULAR_BUFFER_HPP