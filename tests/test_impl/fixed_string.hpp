#ifndef TEST_IMPL_FIXED_SIZE_STRING_HPP
#define TEST_IMPL_FIXED_SIZE_STRING_HPP

// template< unsigned long _Size, class _CharT, class _Traits = std::char_traits<_CharT> >
// class basic_fixed_string
// {
//   public:
//     using traits_type = _Traits;
//     using value_type  = typename traits_type::char_type;
//     using size_type   = unsigned long;



//   private:
//     value_type m_buffer[_Size+1];



// };

#include <array>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <stdarg.h> // va_list


#include <iostream>
#include <ostream>

template <size_t _Size>
class fixed_string_buffer
{
  public:

    // Constructs from a C string if provided, or sets to "" if nullptr passed.
    fixed_string_buffer(const char *cstr = nullptr)
    : m_data()
    {
      static_assert(_Size > 0, "fixed_string_buffer of zero size!");

      if(cstr)
      {
        assert(strlen(cstr) < _Size && "fixed_string_buffer too small!");
        strcpy(m_data.data(), cstr);
      }
      else { m_data[0] = '\0'; }
    }

    // Constructs from a fixed_string_buffer of a bigger or equal static size.
    template <size_t _S>
    fixed_string_buffer(const fixed_string_buffer<_S>& rhs)
    {
      static_assert(_S <= _Size, "fixed_string_buffer too small!");
      strcpy(m_data.data(), rhs.c_str());
    }

    // Copies from a C string if provided, or sets to "" if nullptr passed.
    fixed_string_buffer& operator=(const char *cstr)
    {
      static_assert(_Size > 0, "fixed_string_buffer of zero size!");

      if(cstr)
      {
        assert(strlen(cstr) < _Size&& "fixed_string_buffer too small!");
        strcpy(m_data.data(), cstr);
      }
      else { m_data[0] = '\0'; }

      return *this;
    }

    // Copies from a fixed_string_buffer of a bigger or equal static size.
    template <size_t _S>
    fixed_string_buffer& operator=(const fixed_string_buffer<_S>& rhs)
    {
      static_assert(_S <= _Size, "fixed_string_buffer too small!");
      strcpy(m_data.data(), rhs.c_str());
      return *this;
    }

    template <size_t _S>
    fixed_string_buffer& operator=(fixed_string_buffer<_S>&& rhs)
    {
      static_assert(_S == _Size, "fixed_string_buffer move requires equal size!");
      std::move(m_data, rhs.m_data);
      return *this;
    }

    // Returns a C string (always valid).
    const char* c_str() const { return m_data.data(); }

    // Formats a string in a good old printf style.
    void format(const char *format, ...)
    {
      va_list args;
      va_start(args, format);
      vsnprintf (m_data.data(), _Size, format, args );
      va_end(args);
    }

  private:
    std::array<char, _Size> m_data;
};

// The formatting logic for the severity level
template< typename CharT, typename TraitsT, size_t _S >
inline std::basic_ostream< CharT, TraitsT >& operator<<(std::basic_ostream< CharT, TraitsT >& out, fixed_string_buffer<_S>& buf)
{
    out << buf.c_str();
    return out;
}


#endif //TEST_IMPL_FIXED_SIZE_STRING_HPP