#ifndef UNIT_TESTS_QUEUE_TESTS_HPP
#define UNIT_TESTS_QUEUE_TESTS_HPP

namespace queues_test
{
  using size_type  = unsigned long;

  template<class Queue>
  size_type produce(Queue& fifo, const size_type cnt)
  {
    size_type i = 0;
    for(; i < cnt; ++i)
    {
      while( ! fifo.push(i) ) { std::this_thread::yield(); }
    }
    return i;
  }

  template<class Queue>
  size_type produce2(Queue& fifo, const size_type cnt)
  {
    size_type i = 0;
    for(; i < cnt; ++i)
    {
      fifo.push(i);
    }
    return i;
  }

  template<class Queue>
  size_type consume(Queue& fifo, const size_type cnt)
  {
    size_type i = 0;
    for(; i < cnt; ++i)
    {
      size_type pop_value;
      while( ! fifo.pop(pop_value) ) { std::this_thread::yield(); }
    }
    return i;
  }
} // queues_test

#endif // UNIT_TESTS_QUEUE_TESTS_HPP