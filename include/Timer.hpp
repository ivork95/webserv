#ifndef TIMER_HPP
#define TIMER_HPP

#include "Socket.hpp"
#include "Client.hpp"
#include <sys/timerfd.h>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h> // must be included

class Client;

class Timer : public Socket
{
public:
    struct itimerspec m_spec
    {
    };
    Client *m_client{};

    // default constructor
    Timer(void) = delete;

    // client constructor
    Timer(Client *client);

    // destructor
    ~Timer(void);
};

#endif