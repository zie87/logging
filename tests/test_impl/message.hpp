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

#endif // TEST_IMPL_MESSAGE_HPP