#ifndef TEST_IMPL_TIMER_HPP
#define TEST_IMPL_TIMER_HPP

#include <chrono>
#include <iostream>

template<class Clock, class Duration = std::chrono::nanoseconds>
struct basic_timer 
{
  typedef Clock    clock_t;
  typedef Duration duration_t;

  basic_timer()
  : clock(), start( clock.now() ) {}

  ~basic_timer() {
    std::chrono::time_point<clock_t> stop = clock.now();
  
    std::cout << "time: " << std::chrono::duration_cast<duration_t>(stop - start).count()
              << std::endl;
  }
  
  private:
    clock_t clock;
    std::chrono::time_point<clock_t>  start;
};
// typedef basic_timer<std::chrono::thread_clock, std::chrono::nanoseconds> thread_timer;
typedef basic_timer<std::chrono::steady_clock, std::chrono::nanoseconds> steady_timer;

#endif // TEST_IMPL_TIMER_HPP