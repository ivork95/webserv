#ifndef ANOTHERPIPE_HPP
#define ANOTHERPIPE_HPP

#include <sys/timerfd.h>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h> // must be included

#include "Socket.hpp"
#include "Client.hpp"

class Client;

class AnotherPipe : public Socket
{
public:
    int pipefd[2]{};
};

#endif