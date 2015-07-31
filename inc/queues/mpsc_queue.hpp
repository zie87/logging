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

// C++ implementation of Dmitry Vyukov's non-intrusive lock free unbound MPSC queue
// http://www.1024cores.net/home/lock-free-algorithms/queues/non-intrusive-mpsc-node-based-queue

#ifndef QUEUES_MPSC_QUEUE_HPP
#define QUEUES_MPSC_QUEUE_HPP

#include <atomic>
#include <assert.h>

template<typename T>
class mpsc_queue
{
  struct buffer_node_t;
  public:
    using value_type  = T;
    using atomic_type = std::atomic<buffer_node_t*>;

    mpsc_queue() 
    : m_head(reinterpret_cast<buffer_node_t*>(new aligned_t))
    , m_tail(m_head.load(std::memory_order_relaxed))
    {
      buffer_node_t* front = m_head.load(std::memory_order_relaxed);
      front->next.store(nullptr, std::memory_order_relaxed);
    }

    ~mpsc_queue()
    {
      value_type output;
      while (this->pop(output)) {}
      buffer_node_t* front = m_head.load(std::memory_order_relaxed);
      delete front;
    }

    void push(const value_type& input)
    {
      buffer_node_t* node = reinterpret_cast<buffer_node_t*>(new aligned_t);
      node->data = input;
      node->next.store(nullptr, std::memory_order_relaxed);

      buffer_node_t* prevm_head = m_head.exchange(node, std::memory_order_acq_rel);
      prevm_head->next.store(node, std::memory_order_release);
    }

    bool pop(value_type& output)
    {
      buffer_node_t* tail = m_tail.load(std::memory_order_relaxed);
      buffer_node_t* next = tail->next.load(std::memory_order_acquire);

      if (next == nullptr) { return false; }

      output = next->data;
      m_tail.store(next, std::memory_order_release);
      delete tail;
      return true;
    }


    mpsc_queue(const mpsc_queue&)            = delete;
    mpsc_queue& operator=(const mpsc_queue&) = delete;

  private:
    struct buffer_node_t
    {
        value_type                           data;
        atomic_type next;
    };

    using aligned_t = typename std::aligned_storage<sizeof(buffer_node_t), std::alignment_of<buffer_node_t>::value>::type;

    atomic_type m_head;
    atomic_type m_tail;
};

#endif
