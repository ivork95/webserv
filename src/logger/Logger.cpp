
#include "Logger.hpp"

Logger::Logger(const std::string& filename) : _logFile(filename, std::ios::app) {
	// std::cout << "Logger constructor called\n";
}

Logger::~Logger(void) {
	// std::cout << "Logger destructor called\n";
}

std::string Logger::_getTimestamp() {
	std::time_t now = std::time(nullptr);
	char timestamp[20];
	std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
	return timestamp;
}

void	Logger::log(const std::string &message) {
	std::string logMsg = "[" + _getTimestamp() + "] ";
	logMsg.append("\033[1;32m[INFO]\033[0m ");
	logMsg.append(message);
	std::cout << logMsg << std::endl;
	_logFile << logMsg << std::endl;
}

void	Logger::debug(const std::string &message) {
	std::string logMsg = "[" + _getTimestamp() + "] ";
	logMsg.append("\033[1;34m[DEBUG]\033[0m ");
	logMsg.append(message);
	std::cerr << logMsg << std::endl;
	_logFile << logMsg << std::endl;
}

void	Logger::error(const std::string &message) {
	std::string logMsg = "[" + _getTimestamp() + "] ";
	logMsg.append("\033[1;31m[ERROR]\033[0m ");
	logMsg.append(message);
	std::cerr << logMsg << std::endl;
	_logFile << logMsg << std::endl;
}