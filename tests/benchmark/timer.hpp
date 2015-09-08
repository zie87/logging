#ifndef PROTOTYO_TIMER_HPP
#define PROTOTYO_TIMER_HPP


#include <vector>
#include <chrono>

namespace benchmark
{

  template<class Clock, class Duration = std::chrono::nanoseconds>
  struct basic_timer 
  {
    typedef Clock    clock_t;
    typedef Duration duration_t;
    typedef std::vector<std::size_t> vector_t;

    basic_timer(vector_t& vec)
    : clock(), start( clock.now() ), time_vector(vec) {}

    ~basic_timer() {
      std::chrono::time_point<clock_t> stop = clock.now();
      time_vector.push_back( std::chrono::duration_cast<duration_t>(stop - start).count() );
    }
    
    private:
      clock_t clock;
      std::chrono::time_point<clock_t>  start;
      vector_t& time_vector;
  };
  typedef basic_timer<std::chrono::steady_clock, std::chrono::nanoseconds> steady_timer;


} // benchmark

#endif // PROTOTYO_TIMER_HPP