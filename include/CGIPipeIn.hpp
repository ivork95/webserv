#ifndef CGIPIPEIN_HPP
#define CGIPIPEIN_HPP

class CGIPipeIn : public Socket
{
public:
    int m_pipeFd[2]{};
};

#endif