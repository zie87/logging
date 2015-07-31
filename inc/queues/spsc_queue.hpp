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


// non-intrusive lock free unbounded SPSC queue
// the algorithm was taken from the blog post below, and converted to C++11
// http://cbloomrants.blogspot.com/2009/02/02-26-09-low-level-threading-part-51.html

#ifndef QUEUES_SPSC_QUEUE_HPP
#define QUEUES_SPSC_QUEUE_HPP

#include <atomic>

template<typename T>
class spsc_queue
{
  public:
    using value_type = T;

    spsc_queue() 
    : m_head(reinterpret_cast<node_t*>(new aligned_t))
    , m_tail(m_head), m_back(nullptr)
    {
      m_head->next = nullptr;
    }

    ~spsc_queue()
    {
      value_type output;
      while (this->pop(output)) {}
      delete m_head;
    }

    void push(const value_type& input)
    {
      node_t* node = reinterpret_cast<node_t*>(new aligned_t);
      node->data = input;
      node->next = nullptr;

      std::atomic_thread_fence(std::memory_order_acq_rel);
      m_head->next = node;
      m_head = node;
    }

    bool pop(value_type& output)
    {
      std::atomic_thread_fence(std::memory_order_consume);
      if (!m_tail->next) { return false; }

      output = m_tail->next->data;
      std::atomic_thread_fence(std::memory_order_acq_rel);
      m_back = m_tail;
      m_tail = m_back->next;

      delete m_back;
      return true;
    }

    spsc_queue(const spsc_queue&)            = delete;
    spsc_queue& operator=(const spsc_queue&) = delete;

  private:

    struct node_t
    {
      node_t*     next;
      value_type  data;
    };

    using aligned_t =  typename std::aligned_storage<sizeof(node_t), std::alignment_of<node_t>::value>::type;
    typedef char cache_line_pad_t[64]; // it's either 32 or 64 so 64 is good enough

    node_t*          m_head;
    cache_line_pad_t m_pad;
    node_t*          m_tail;
    node_t*          m_back;
};

#endif
