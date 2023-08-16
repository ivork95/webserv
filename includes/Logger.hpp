#include <iostream>
#include <fstream>
#include <string>
#include <ctime>

enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR
};

class Logger {
	public:
		Logger(const std::string& filename);
		~Logger(void);

		void log(const std::string &message);
		void debug(const std::string &message);
		void error(const std::string &message);

	private:
		std::ofstream _logFile;

		std::string _getTimestamp();
};
