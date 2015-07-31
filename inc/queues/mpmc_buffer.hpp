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

// Implementation of Dmitry Vyukov's MPMC algorithm
// http://www.1024cores.net/home/lock-free-algorithms/queues/bounded-mpmc-queue


#ifndef QUEUES_MBMC_BUFFER_HPP
#define QUEUES_MBMC_BUFFER_HPP

#include <atomic>
#include <assert.h>

template<typename T>
class mpmc_buffer
{
  public:
    using size_type   = size_t;
    using atomic_type = std::atomic<size_type>;
    using value_type  = T;

    mpmc_buffer( size_type size ) 
    : m_size(size), m_mask(size - 1)
    , m_buf(reinterpret_cast<node_t*>(new aligned_t[m_size]))
    , m_head_seq(0), m_tail_seq(0)
    {
      // make sure it's a power of 2
      assert((m_size != 0) && ((m_size & (~m_size + 1)) == m_size));
      // populate the sequence initial values
      for (size_type i = 0; i < m_size; ++i) {m_buf[i].seq.store(i, std::memory_order_relaxed);}
    }

    ~mpmc_buffer(){ if(m_buf) {delete[] m_buf;} }

    bool push(const value_type& data) noexcept
    {
      // m_head_seq only wraps at MAX(m_head_seq) instead we use a mask to convert the sequence to an array index
      // this is why the ring buffer must be a size which is a power of 2. this also allows the sequence to double as a ticket/lock.
      size_type  head_seq = m_head_seq.load(std::memory_order_relaxed);

      for (;;) 
      {
        node_t*   node     = &m_buf[head_seq & m_mask];
        size_type node_seq = node->seq.load(std::memory_order_acquire);
        intptr_t  dif      = (intptr_t) node_seq - (intptr_t) head_seq;
        // if seq and head_seq are the same then it means this slot is empty
        if (dif == 0) 
        {
          // claim our spot by moving head
          // if head isn't the same as we last checked then that means someone beat us to the punch
          // weak compare is faster, but can return spurious results
          // which in this instance is OK, because it's in the loop
          if (m_head_seq.compare_exchange_weak(head_seq, head_seq + 1, std::memory_order_relaxed))
          {
            // set the data
            node->data = data;
            // increment the sequence so that the tail knows it's accessible
            node->seq.store(head_seq + 1, std::memory_order_release);
            return true;
          }
        }
        else if (dif < 0) 
        {
          // if seq is less than head seq then it means this slot is full and therefore the buffer is full
          return false;
        }
        else 
        {
          // under normal circumstances this branch should never be taken
          head_seq = m_head_seq.load(std::memory_order_relaxed);
        }
      }
      return false; // never taken
    }

    bool pop(value_type& data) noexcept
    {
      size_type tail_seq = m_tail_seq.load(std::memory_order_relaxed);

      for (;;)
      {
        node_t*   node     = &m_buf[tail_seq & m_mask];
        size_type node_seq = node->seq.load(std::memory_order_acquire);
        intptr_t  dif      = (intptr_t) node_seq - (intptr_t)(tail_seq + 1);

        // if seq and head_seq are the same then it means this slot is empty
        if (dif == 0) 
        {
          // claim our spot by moving head
          // if head isn't the same as we last checked then that means someone beat us to the punch
          // weak compare is faster, but can return spurious results
          // which in this instance is OK, because it's in the loop
          if (m_tail_seq.compare_exchange_weak(tail_seq, tail_seq + 1, std::memory_order_relaxed)) 
          {
            // set the output
            data = node->data;
            // set the sequence to what the head sequence should be next time around
            node->seq.store(tail_seq + m_mask + 1, std::memory_order_release);
            return true;
          }
        }
        else if (dif < 0) 
        {
          // if seq is less than head seq then it means this slot is full and therefore the buffer is full
          return false;
        }
        else 
        {
          // under normal circumstances this branch should never be taken
          tail_seq = m_tail_seq.load(std::memory_order_relaxed);
        }
      }
      return false; // never taken
    }

    mpmc_buffer(const mpmc_buffer&)            = delete;
    mpmc_buffer& operator=(const mpmc_buffer&) = delete;

private:

    struct node_t
    {
      value_type    data;
      atomic_type   seq;
    };

    // prevent false sharing (http://stackoverflow.com/questions/8469427/how-and-when-to-align-to-cache-line-size)
    using aligned_t = typename std::aligned_storage<sizeof(node_t), std::alignment_of<node_t>::value>::type;
    typedef char cache_line_pad_t[64]; // it's either 32 or 64 so 64 is good enough

    cache_line_pad_t  m_pad0;
    const size_type   m_size;
    const size_type   m_mask;
    node_t* const     m_buf;
    cache_line_pad_t  m_pad1;
    atomic_type       m_head_seq;
    cache_line_pad_t  m_pad2;
    atomic_type       m_tail_seq;
    cache_line_pad_t  m_pad3;
};

#endif // QUEUES_MBMC_BUFFER_HPP
