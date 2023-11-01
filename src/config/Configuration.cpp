
#include "Configuration.hpp"

/**
 * CONSTRUCTORS / DESTRUCTORS
*/
Configuration::Configuration(void) : serversConfig(), serverSections() {
	Logger::getInstance().debug("Configuration default constructor called");
}

Configuration::~Configuration(void) {
	Logger::getInstance().debug("Configuration destructor called");
}

/**
 * MEMBER FUNCTIONS
*/
