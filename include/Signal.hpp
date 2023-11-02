#ifndef SIGNAL_HPP
#define SIGNAL_HPP

#include "sys/signalfd.h"
#include "ASocket.hpp"

class Signal : public ASocket
{
private:
    /* data */
public:
    Signal(/* args */);
    ~Signal();

    struct signalfd_siginfo fdsi;
};

Signal::Signal(/* args */)
{
    sigset_t mask;

    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGQUIT);

    /* Block signals so that they aren't handled
       according to their default dispositions. */
    if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1)
        throw std::runtime_error("Error: sigprocmask()");

    m_socketFd = signalfd(-1, &mask, 0);
    if (m_socketFd == -1)
        throw std::runtime_error("Error: signalfd()");
}

Signal::~Signal()
{
}

#endif