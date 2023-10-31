
#include "Logger.hpp"

/**
 * CONSTRUCTORS / DESTRUCTORS
*/
Logger::Logger() {}

Logger::~Logger() {}

/**
 * STATIC FUNCTIONS
*/
static std::string getCurrentTime() {
    auto now = std::time(nullptr);
    auto tm_time = *std::localtime(&now);

    // Get milliseconds using a platform-specific method
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    int milliseconds = ts.tv_nsec / 1000000;

    std::ostringstream oss;
    oss << std::put_time(&tm_time, "%H:%M:%S") << ":" << std::setfill('0') << std::setw(3) << milliseconds;
    return oss.str();
}

/**
 * SETTERS / GETTERS
*/
Logger &Logger::getInstance() {
	static Logger instance;
	return instance;
}

/**
 * MEMBER FUNCTIONS
*/
void Logger::log(const std::string &message) {
	std::cout << _getTimeStamp() << " [" << GREEN << "INFO" << RC << "] " << message << std::endl;
}

void Logger::debug(const std::string &message) {
	std::cout << _getTimeStamp() << " [" << BLUE << "DEBUG" << RC << "] " << message << std::endl;
}

void Logger::error(const std::string &message) {
	std::cout << _getTimeStamp() << " [" << RED << "ERROR" << RC << "] " << message << std::endl;
}

std::string Logger::_getTimeStamp() const {
	std::string timeStamp = "[";
	std::string currentTime = getCurrentTime();
	timeStamp.append(currentTime);
	timeStamp.append("]");
	return timeStamp;
}
