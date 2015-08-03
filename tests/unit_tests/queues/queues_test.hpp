#ifndef UNIT_TESTS_QUEUE_TESTS_HPP
#define UNIT_TESTS_QUEUE_TESTS_HPP

#include <iostream>

namespace queues_test
{
  using size_type  = unsigned long;


  template<class Queue>
  size_type buffer_produce(Queue& fifo, const size_type cnt, typename Queue::value_type* val_array)
  {
    using value_type = typename Queue::value_type;
    size_type i = 0;
    value_type val = value_type();
    for(; i < cnt; ++i)
    {
      // loop is necessary -> on "lock" is none msg produced so the consumer 
      // would get infinity calls
      while( ! fifo.push( val_array[i] ) ) { std::this_thread::yield(); }
    }
    return i;
  }

  template<class Queue>
  size_type queue_produce(Queue& fifo, const size_type cnt, typename Queue::value_type* val_array)
  {
    using value_type = typename Queue::value_type;
    size_type i = 0;
    value_type val = value_type();
    for(; i < cnt; ++i)
    {
      fifo.push( val_array[i] );
    }
    return i;
  }

  template<class Queue>
  size_type consume(Queue& fifo, const size_type cnt)
  {
    using value_type = typename Queue::value_type;
    size_type i = 0;
    for(; i < cnt; ++i)
    {
      value_type pop_value;
      while( ! fifo.pop(pop_value) ) { std::this_thread::yield(); }
    }
    return i;
  }
} // queues_test

#endif // UNIT_TESTS_QUEUE_TESTS_HPP