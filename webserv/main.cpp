#include "TcpServer.hpp"

/*
1. clang++ main.cpp -o server
2. ./server
3. new terminal window -> clang++ gptExampleClient.cpp -o client
4. ./client
5. celebrate
*/

int main(void)
{
    TcpServer t;
    t.sendData();
}
