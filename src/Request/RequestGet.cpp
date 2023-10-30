#include "Request.hpp"
#include "Multiplexer.hpp"
#include "CGIPipeIn.hpp"
#include "CGIPipeOut.hpp"

int	Request::directoryListingHandler(void) {
	spdlog::info("No index file, looking for autoindex: {}", m_locationconfig.getRootPath());

	const std::string dirPath = directoryListingParse();
	if (dirPath.empty())
		throw StatusCodeException(500, "Error: directoryListingParse");
	if (!std::filesystem::is_directory(dirPath))
		throw StatusCodeException(404, "Error: no matching index file found");
	else
	{
		if (!m_locationconfig.getAutoIndex())
			throw StatusCodeException(403, "Forbidden: directory listing disabled");

		spdlog::info("Found autoindex: {}", dirPath);
		directoryListingBodySet(dirPath);
		return 0;
	}
	return 2; // ? not there originally
}

int Request::uploadHandler(void) {
	m_response.bodySet("./www" + m_methodPathVersion[1]);
	m_response.m_statusCode = 200;
	return 0;
}

int Request::getHandler(void) {
	// Nasty solution to redirect + get back upload
	if (m_methodPathVersion[1].ends_with("jpg") || m_methodPathVersion[1].ends_with("jpeg") || m_methodPathVersion[1].ends_with("png"))
	{
		return uploadHandler();
	}

	// Can't find an index file, check if directory listing
	if (m_response.m_path.empty())
	{
		return directoryListingHandler();
	}

	m_response.bodySet(m_response.m_path);
	m_response.m_statusCode = 200;
	return 0;
}