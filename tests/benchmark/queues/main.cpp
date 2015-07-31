#include <iostream>
#include <iomanip>
#include <thread>

#include <cmath>

#include <queues/spsc_queue.hpp>
#include <queues/mpsc_queue.hpp>
#include <queues/spsc_buffer.hpp>
#include <queues/mpmc_buffer.hpp>

static const unsigned long counter    = 100000ul;
static const unsigned int  iterations = 100;

typedef long double measurements[iterations];
static const unsigned int  test_items = 4;

template<typename T>
void consumer_func(T* queue)
{
  size_t count = counter;
  size_t value = 0;

  while (count > 0) { if (queue->pop(value)) { --count; } }
}

template<typename T>
void bounded_producer_func(T* queue)
{
  size_t count = counter;
  while (count > 0) { if (queue->push(count)) { --count; } }
}

template<typename T>
void producer_func(T* queue)
{
  for (unsigned int count = 0; count < counter; ++count) { queue->push(count); }
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

 std::chrono::duration<long double> diff = (end - start);
 long double sec = diff.count();
  return sec;
}

long double measurement_variance_calculation(const long double& avg, const measurements& measure )
{
  long double sum = 0;
  for(unsigned int i = 0; i < iterations; ++ i)
  {
    long double current = measure[i] - avg;
    sum += current*current;
  }

  long double variance = sum/( iterations -1 );
  return variance; 
}

void print_measurements(const std::string& name, const measurements& measure )
{
  long double max = 0;
  long double min = measure[0];
  long double sum = 0;
  long double avg = 0;

  for(unsigned long int i = 0; i < iterations; ++ i)
  {
    long double current = measure[i];
    if( current < min ) min = current;
    if( current > max ) max = current;
    sum += current;
  }

  avg = sum / iterations;
  long double dev = sqrt( measurement_variance_calculation(avg, measure) );
  long double err = dev/sqrt(iterations);

  std::cout << name << " completed with "<< counter << " iterations and " << iterations << " measurements" << std::endl
            << "time for completion: " << sum << " seconds." << std::endl
            << "min time: " << std::setw(15) << std::right << std::setprecision(5) << std::fixed << min*1000 << " ms" <<  std::endl
            << "max time: " << std::setw(15) << std::right << std::setprecision(5) << std::fixed << max*1000 << " ms" <<  std::endl
            << "avg time: " << std::setw(15) << std::right << std::setprecision(5) << std::fixed << avg*1000 << " ms" <<  std::endl
            << "dev time: " << std::setw(15) << std::right << std::setprecision(5) << std::fixed << dev*1000 << " ms" <<  std::endl
            << "err time: " << std::setw(15) << std::right << std::setprecision(5) << std::fixed << err*1000 << " ms" <<  std::endl
            << "push/pop pairs per second: " << ((long double) counter / avg) / 1000000 << " million" << std::endl
            << std::endl;
}

int main()
{
  measurements time_array[test_items];
  unsigned int current_item = 0;

  {
    std::string name("SPSC circular buffer");
    std::cout << "Starting tests for " << name << std::endl;
    typedef spsc_buffer<size_t> queue_t;
    for( unsigned int i = 0; i < iterations; ++i )
    {
      queue_t queue(65536);
      time_array[current_item][i] = run_test(std::bind(&bounded_producer_func<queue_t>, &queue), 
                                             std::bind(&consumer_func<queue_t>, &queue)); 
    }
    print_measurements(name, time_array[current_item++]);
  }

  {
    std::string name("MPMC circular buffer");
    std::cout << "Starting tests for " << name << std::endl;
    typedef mpmc_buffer<size_t> queue_t;
    for( unsigned int i = 0; i < iterations; ++i )
    {
      queue_t queue(65536);
      time_array[current_item][i] = run_test(std::bind(&bounded_producer_func<queue_t>, &queue),
                                             std::bind(&consumer_func<queue_t>, &queue)); 
    }
    print_measurements(name, time_array[current_item++]);
  }

  {
    std::string name("SPSC dynamic queue");
    std::cout << "Starting tests for " << name << std::endl;
    typedef spsc_queue<size_t> queue_t;
    for( unsigned int i = 0; i < iterations; ++i )
    {
      queue_t queue;
      time_array[current_item][i] = run_test(std::bind(&producer_func<queue_t>, &queue),
                                             std::bind(&consumer_func<queue_t>, &queue));
    }
    print_measurements(name, time_array[current_item++]);
  }

  {
    std::string name("MPSC dynamic queue");
    std::cout << "Starting tests for " << name << std::endl;
    typedef mpsc_queue<size_t> queue_t;
    for( unsigned int i = 0; i < iterations; ++i )
    {
      queue_t queue;
      time_array[current_item][i] = run_test(std::bind(&producer_func<queue_t>, &queue),
                                             std::bind(&consumer_func<queue_t>, &queue));
    }
    print_measurements(name, time_array[current_item++]);
  }

  return 0;
}
