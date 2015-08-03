#ifndef TEST_IMPL_TIMER_HPP
#define TEST_IMPL_TIMER_HPP

#include <chrono>
#include <iostream>
#include <vector>

template<class Clock, class Duration = std::chrono::nanoseconds>
struct basic_timer 
{
  typedef Clock    clock_t;
  typedef Duration duration_t;

  basic_timer()
  : clock(), start( clock.now() ) 
  {}

  ~basic_timer() 
  {
    std::chrono::time_point<clock_t> stop = clock.now();
    std::cout << "time: " << std::chrono::duration_cast<duration_t>(stop - start).count()
              << std::endl;
  }
  
  private:
    clock_t clock;
    std::chrono::time_point<clock_t>  start;
};
typedef basic_timer<std::chrono::steady_clock, std::chrono::nanoseconds> steady_timer;

template<class Clock, class Duration = std::chrono::nanoseconds>
struct basic_vector_timer 
{
  typedef Clock    clock_t;
  typedef Duration duration_t;
  typedef std::vector<std::size_t> vector_t;

  basic_vector_timer(vector_t& vec)
  : clock(), start( clock.now() ), time_vector(vec) 
  {}

  ~basic_vector_timer() 
  {
    std::chrono::time_point<clock_t> stop = clock.now();
    time_vector.push_back(std::chrono::duration_cast<duration_t>(stop - start).count() );
  }
  
  private:
    clock_t clock;
    std::chrono::time_point<clock_t>  start;
    vector_t& time_vector;
};
typedef basic_vector_timer<std::chrono::steady_clock, std::chrono::nanoseconds> steady_vector_timer;

#endif // TEST_IMPL_TIMER_HPP