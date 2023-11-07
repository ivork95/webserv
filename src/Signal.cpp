#include "Signal.hpp"

Signal::Signal(void)
{
    std::cout << "Signal default constructor called\n";

    sigset_t mask{};

    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGQUIT);

    /* Block signals so that they aren't handled
       according to their default dispositions. */
    if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1)
        throw std::system_error(errno, std::generic_category(), "sigprocmask()");

    m_socketFd = signalfd(-1, &mask, 0);
    if (m_socketFd == -1)
        throw std::system_error(errno, std::generic_category(), "signalfd()");
}

Signal::~Signal()
{
    std::cout << "Signal destructor called" << std::endl;

    close(m_socketFd);
}