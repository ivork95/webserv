#include "Signal.hpp"
#include "Multiplexer.hpp"
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

Signal::Signal(void)
{
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
    close(m_socketFd);
}

void Signal::readAndDelete(void) const
{
    Multiplexer &multiplexer = Multiplexer::getInstance();
    struct signalfd_siginfo fdsi
    {
    };

    ssize_t s = read(m_socketFd, &fdsi, sizeof(fdsi));
    if (s != sizeof(fdsi))
        throw std::system_error(errno, std::generic_category(), "read()");
    if (fdsi.ssi_signo == SIGINT || fdsi.ssi_signo == SIGQUIT)
    {
        spdlog::info("Exiting Webserver");

        for (auto &server : multiplexer.m_servers)
            delete server;
        for (auto &client : multiplexer.m_clients)
            delete client;
        multiplexer.m_isRunning = false;
    }
    else
        std::cerr << "Unhandeled signal received. Continuing...\n";
}