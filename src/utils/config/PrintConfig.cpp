
#include "Configuration.hpp"

void	Configuration::printConfig(void) {
	std::cout << "\n\t\t -----------------\n \t\t|  SERVER CONFIG  |\n\t\t -----------------\n";
    for (size_t i = 0; i < serversConfig.size(); ++i)
    {
        std::cout << serversConfig[i];
    }
    return ;
}