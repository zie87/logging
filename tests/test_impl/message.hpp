#ifndef TEST_IMPL_MESSAGE_HPP
#define TEST_IMPL_MESSAGE_HPP

  struct message
  {
    using string_type = std::string;

    message(const int& i = 0)
    : item(i)
    , channel("NO_CHANNEL")
    {}

    message(const char* chan,const int& i = 0)
    : item(i)
    , channel(chan)
    {}

    message(const message& msg)
    : item( msg.item )
    , channel( msg.channel )
    {}

    unsigned int      item;
    const char*       channel; 
  };

  /*
  struct message
  {
    using string_type = std::string;

    message()
    : channel(), item()
    {}

    message(const std::string& chan, const int& i = 0)
    : channel(chan), item(i)
    {}

    string_type       channel; 
    unsigned int      item;
  };
  */

#endif // TEST_IMPL_MESSAGE_HPP