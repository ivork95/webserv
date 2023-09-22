#ifndef TIMER_HPP
#define TIMER_HPP

#include <sys/timerfd.h>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h> // must be included

#include "Socket.hpp"
#include "Client.hpp"

class Client;

class Timer : public Socket
{
public:
    Client *m_client{};
    struct itimerspec m_spec
    {
    };

    // default constructor
    Timer(void) = delete;

    // client constructor
    Timer(Client *client);

    // destructor
    ~Timer(void);

    // outstream operator overload
    friend std::ostream &operator<<(std::ostream &out, const Timer &timer);
};

#endif