

#include <vector>
#include <iostream>

#include "sink.hpp"
#include "source.hpp"
#include "core.hpp"


std::vector<message> message_vector;
class vector_sink : public sink
{
  public:
    using basic_type = sink;
    using size_type  = typename basic_type::size_type;
    using value_type = typename basic_type::value_type;


    explicit vector_sink(size_type size = 65536)
    : basic_type(size)
    {}

    vector_sink(const vector_sink&)            = delete;
    vector_sink& operator=(const vector_sink&) = delete;

  protected:
    void consume(const value_type& val) override { message_vector.push_back(val); }
};

int main()
{
  const unsigned int count = 100000;
  using sink_ptr = typename core::sink_ptr;

  sink_ptr  _sink( new vector_sink );
  source             _source;

  auto& core_inst = core::instance();

  core_inst.insert_sink(_sink);

  // _source.start();
  // _sink->start();

  for(unsigned int i = 0; i < count; ++i) { _source.log(i); }

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  core_inst.remove_sink(_sink);
  
  // _sink.stop();
  // _sink->flush();
  std::cout << "size " << message_vector.size() << " should be " << count << std::endl;
  _sink->flush();
  // auto vec = dynamic_cast<vector_sink*>(_sink.get())->vector();

  for(unsigned int i = 0; i < message_vector.size(); ++i) 
  { 
    std::cout << message_vector[i].item << "\t"; 
    if( i%20 == 0 ) std::cout << std::endl;
  }


  std::cout << std::endl << "size " << message_vector.size() << " should be " << count << std::endl;


  return 0;
}