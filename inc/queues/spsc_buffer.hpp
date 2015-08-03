// This is free and unencumbered software released into the public domain.

// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.

// In jurisdictions that recognize copyright laws, the author or authors
// of this software dedicate any and all copyright interest in the
// software to the public domain. We make this dedication for the benefit
// of the public at large and to the detriment of our heirs and
// successors. We intend this dedication to be an overt act of
// relinquishment in perpetuity of all present and future rights to this
// software under copyright law.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.

// For more information, please refer to <http://unlicense.org/>

// Note:
// A combination of the algorithms described by the circular buffers
// documentation found in the Linux kernel, and the bounded MPMC queue
// by Dmitry Vyukov[1]. Implemented in pure C++11. Should work across
// most CPU architectures.
//
// [1] http://www.1024cores.net/home/lock-free-algorithms/queues/bounded-mpmc-queue

#ifndef QUEUES_SPSC_BUFFER_HPP
#define QUEUES_SPSC_BUFFER_HPP

#include <atomic>
#include <assert.h>

template<typename T>
class spsc_buffer
{
  public:
    using size_type   = size_t;
    using atomic_type = std::atomic<size_type>;
    using value_type  = T;

    spsc_buffer( size_type size) 
    : m_size(size), m_mask(size - 1)
    , m_buf(reinterpret_cast<value_type*>(new aligned_t[m_size + 1])) // need one extra element for a guard 
    , m_head(0), m_tail(0)
    {
      // make sure it's a power of 2
      assert((m_size != 0) && ((m_size & (~m_size + 1)) == m_size));
    }

    ~spsc_buffer() { delete[] m_buf; }

    bool push(const value_type& input) noexcept
    {
      const size_type head = m_head.load(std::memory_order_relaxed);

      if (((m_tail.load(std::memory_order_acquire) - (head + 1)) & m_mask) >= 1) 
      {
        m_buf[head & m_mask] = input;
        m_head.store(head + 1, std::memory_order_release);
        return true;
      }
      return false;
    }

    bool push(value_type&& input) noexcept
    {
      const size_type head = m_head.load(std::memory_order_relaxed);

      if (((m_tail.load(std::memory_order_acquire) - (head + 1)) & m_mask) >= 1) 
      {
        m_buf[head & m_mask] = std::move(input);
        m_head.store(head + 1, std::memory_order_release);
        return true;
      }
      return false;
    }

    bool pop(value_type& output) noexcept
    {
      const size_type tail = m_tail.load(std::memory_order_relaxed);

      if (((m_head.load(std::memory_order_acquire) - tail) & m_mask) >= 1) 
      {
      //   output = m_buf[m_tail & m_mask];
        output = std::move( m_buf[m_tail & m_mask] );
        m_tail.store(tail + 1, std::memory_order_release);
        return true;
      }
      return false;
    }

    spsc_buffer(const spsc_buffer&)            = delete;
    spsc_buffer& operator=(const spsc_buffer&) = delete;

private:
    using aligned_t =  typename std::aligned_storage<sizeof(value_type), std::alignment_of<value_type>::value>::type;
    typedef char cache_line_pad_t[64];

    cache_line_pad_t    m_pad0;
    const size_type     m_size;
    const size_type     m_mask;
    value_type* const   m_buf;
    cache_line_pad_t    m_pad1;
    atomic_type         m_head;
    cache_line_pad_t    m_pad2;
    atomic_type         m_tail;
};

#endif //QUEUES_SPSC_BUFFER_HPP
