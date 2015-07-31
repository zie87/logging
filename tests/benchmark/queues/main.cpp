#include <iostream>
#include <thread>

#include <queues/spsc_queue.hpp>
#include <queues/mpsc_queue.hpp>
#include <queues/spsc_buffer.hpp>
#include <queues/mpmc_buffer.hpp>

static const unsigned long counter = 100000000;

template<typename T>
void consumer_func(T* queue)
{
  size_t count = counter;
  size_t value = 0;

  while (count > 0) 
  {
    if (queue->dequeue(value)) { --count; }
  }
}

template<typename T>
void bounded_producer_func(T* queue)
{
  size_t count = counter;
  while (count > 0) 
  {
    if (queue->enqueue(count)) { --count; }
  }
}

template<typename T>
void producer_func(T* queue)
{
  for (unsigned int count = 0; count < counter; ++count) { queue->enqueue(count); }
}

template<typename T>
long double run_test(T producer_func, T consumer_func)
{
  typedef std::chrono::high_resolution_clock clock_t;
  typedef std::chrono::time_point<clock_t> time_t;
  time_t start;
  time_t end;

  start = clock_t::now();
  std::thread producer(producer_func);
  std::thread consumer(consumer_func);

  producer.join();
  consumer.join();
  end = clock_t::now();

  long double sec =  (end - start).count() * ((double) std::chrono::high_resolution_clock::period::num 
                                                / std::chrono::high_resolution_clock::period::den);
  return sec;
}

int
main()
{
    {
        typedef spsc_buffer<size_t> queue_t;
        queue_t queue(65536);
        long double seconds = run_test(std::bind(&bounded_producer_func<queue_t>, &queue),
                                       std::bind(&consumer_func<queue_t>, &queue));

        std::cout << "SPSC circular buffer completed "
                  << counter
                  << " iterations in "
                  << seconds
                  << " seconds. "
                  << ((long double) counter / seconds) / 1000000
                  << " million enqueue/dequeue pairs per second."
                  << std::endl;
    }

    {
        typedef mpmc_buffer<size_t> queue_t;
        queue_t queue(65536);
        long double seconds = run_test(std::bind(&bounded_producer_func<queue_t>, &queue),
                                       std::bind(&consumer_func<queue_t>, &queue));

        std::cout << "MPMC circular buffer completed "
                  << counter
                  << " iterations in "
                  << seconds
                  << " seconds. "
                  << ((long double) counter / seconds) / 1000000
                  << " million enqueue/dequeue pairs per second."
                  << std::endl;
    }

    {
        typedef spsc_queue<size_t> queue_t;
        queue_t queue;
        long double seconds = run_test(std::bind(&producer_func<queue_t>, &queue),
                                       std::bind(&consumer_func<queue_t>, &queue));

        std::cout << "SPSC dynamic queue completed "
                  << counter
                  << " iterations in "
                  << seconds
                  << " seconds. "
                  << ((long double) counter / seconds) / 1000000
                  << " million enqueue/dequeue pairs per second."
                  << std::endl;
    }

    {
        typedef mpsc_queue<size_t> queue_t;
        queue_t queue;
        long double seconds = run_test(std::bind(&producer_func<queue_t>, &queue),
                                       std::bind(&consumer_func<queue_t>, &queue));

        std::cout << "MPSC dynamic queue completed "
                  << counter
                  << " iterations in "
                  << seconds
                  << " seconds. "
                  << ((long double) counter / seconds) / 1000000
                  << " million enqueue/dequeue pairs per second."
                  << std::endl;
    }

    return 0;
}
