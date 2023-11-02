#ifndef TIMER_HPP
#define TIMER_HPP

#include <fcntl.h>
#include <sys/timerfd.h>

// #include "Client.hpp" // ! this include generates an error
#include "Socket.hpp"

#include "Logger.hpp"

class Client;

class Timer : public Socket
{
public:
    Client &m_client;
    struct itimerspec m_spec
    {
    };

    // default constructor
    Timer(void) = delete;

    // client constructor
    Timer(Client &client);

    // destructor
    ~Timer(void);

    // outstream operator overload
    friend std::ostream &operator<<(std::ostream &out, const Timer &timer);
};

#endif