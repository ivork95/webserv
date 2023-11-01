#include "Request.hpp"
#include "Multiplexer.hpp"
#include "CGIPipeIn.hpp"
#include "CGIPipeOut.hpp"

int Request::uploadHandler(void) {
	// spdlog::warn("GET upload handler"); // ? debug
	Logger::getInstance().debug("GET upload handler");

	m_response.bodySet("./www" + m_methodPathVersion[1]);
	m_response.m_statusCode = 200;
	return 0;
}

int Request::getHandler(void) {
	Logger &logger = Logger::getInstance();

	// spdlog::warn("GET handler"); // ? debug
	logger.debug("GET handler");

	// Can't find an index file, check if directory listing
	if (m_response.m_path.empty())
	{
		// spdlog::info("No index file, looking for autoindex: {}", m_locationconfig.getRootPath());
		// spdlog::warn("GET dir listing handler"); // ? debug
		logger.debug("No index file, looking for autoindex: " + m_locationconfig.getRootPath());
		logger.debug("GET dir listing handler");

		const std::string dirPath = directoryListingParse();
		if (dirPath.empty())
			throw StatusCodeException(500, "Error: directoryListingParse");
		if (!std::filesystem::is_directory(dirPath))
			throw StatusCodeException(404, "Error: no matching index file found");
		else
		{
			if (!m_locationconfig.getAutoIndex())
				throw StatusCodeException(403, "Forbidden: directory listing disabled");

			// spdlog::info("Found autoindex: {}", dirPath);
			logger.debug("Found autoindex: " + dirPath);
			directoryListingBodySet(dirPath);
			return 0;
		}
	}

	m_response.bodySet(m_response.m_path);
	m_response.m_statusCode = 200;
	return 0;
}