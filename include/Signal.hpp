#ifndef SIGNAL_HPP
#define SIGNAL_HPP

#include <vector>
#include <err.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/signalfd.h>
#include <unistd.h>

#include "ASocket.hpp"

class Signal : public ASocket
{
private:
    /* data */
public:
    // default constructor
    Signal(void);

    // copy constructor
    Signal(const Signal &source) = delete;

    // copy assignment operator overload
    Signal &operator=(const Signal &source) = delete;

    // destructor
    ~Signal(void);

    struct signalfd_siginfo fdsi
    {
    };
};

#endif