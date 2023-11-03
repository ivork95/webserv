#include "Signal.hpp"

Signal::Signal(void)
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
    std::cout << "Signal destructor" << std::endl;

    close(m_socketFd);
}