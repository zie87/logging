#ifndef LOGGING_CONTAINER_CIRCULAR_BUFFER_HPP
#define LOGGING_CONTAINER_CIRCULAR_BUFFER_HPP

namespace container
{

  template<typename T>
  class basic_circular_buffer
  {
    public:
      using size_type  = unsigned long;
      using value_type = T;
      using pointer    = value_type*;

      using reference         =       value_type&;
      using const_reference   = const value_type&;

      explicit basic_circular_buffer(size_type capa);
      ~basic_circular_buffer();

      basic_circular_buffer(const basic_circular_buffer&)            = delete;
      basic_circular_buffer& operator=(const basic_circular_buffer&) = delete;

      reference       front();
      const_reference front() const;

      reference       back();
      const_reference back() const;

      void push( const value_type& val );
      void push( value_type&& val );

      void pop();

      inline size_type size()     const noexcept { return m_size; }
      inline size_type capacity() const noexcept { return m_capacity; }
      inline bool empty() const noexcept { return size() == 0; }
      
      void clean();

    private:
      void construct(pointer adr, const value_type& o);
      void construct(pointer adr, value_type&& o);
      void destroy(pointer adr);

      pointer allocate( size_type s ) const;
      void    deallocate( pointer ptr ) const;

      size_type m_capacity, m_size;
      size_type m_first_pos, m_last_pos;

      pointer   m_data = nullptr;
  };

} // container

#include "internal/circular_buffer.tcc"

#endif // LOGGING_CONTAINER_CIRCULAR_BUFFER_HPP