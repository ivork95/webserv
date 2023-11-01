#ifndef LOGGER_HPP
# define LOGGER_HPP

#include <iostream>
#include <ctime>
#include <sstream>
#include <iomanip>

# define RC "\033[0m"
# define RED "\033[1;31m"
# define GREEN "\033[0;32m"
# define BLUE "\033[1;34m"
# define YELLOW "\033[0;33m"

class Logger {

	public:
		static Logger &getInstance();

		void log(const std::string &message);
		void debug(const std::string &message);
		void error(const std::string &message);
		void warn(const std::string &message);
	
	private:
		Logger();
		~Logger();

		std::string _getTimeStamp() const;

};

#endif
