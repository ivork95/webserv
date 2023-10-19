#ifndef CGIPIPEOUT_HPP
#define CGIPIPEOUT_HPP

class CGIPipeOut : public Socket
{
public:
    int m_pipeFd[2]{};
};

#endif