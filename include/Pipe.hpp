#ifndef PIPE_HPP
#define PIPE_HPP

#include <sys/timerfd.h>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h> // must be included

#include "Socket.hpp"
#include "Client.hpp"

class Client;

class Pipe : public Socket
{
public:
    int pipefd[2]{};
};

#endif