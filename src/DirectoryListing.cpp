#include "HttpRequest.hpp"

static std::string directoryListingGenerate(const std::string &dirPath) {
    std::string listing = "<html><body><h1>Directory Listing</h1><ul>";

    for (const auto &entry : std::filesystem::directory_iterator(dirPath))
    {
        listing += "<li>" + entry.path().filename().string() + "</li>";
    }

    listing += "</ul></body></html>";

    return listing;
}

void	HttpRequest::directoryListingBodySet(const std::string &dirPath) {
	const std::string directoryListing = directoryListingGenerate(dirPath);
	if (directoryListing.empty())
		throw StatusCodeException(400, "Error: directoryListingBodySet");

	// m_response.bodySet(directoryListing); // to change I guess
	m_response.m_body = directoryListing;
	m_response.m_statusCode = 200;
	m_response.m_headers.insert({"Content-Type", "text/html"});
}

std::string	HttpRequest::directoryListingParse(void) {
	std::string requestUri = m_methodPathVersion[1];
	std::string dirPath{};

	// Remove leading '/' before appending root
	if (requestUri[0] == '/')
		requestUri = requestUri.substr(1, requestUri.size());

	dirPath = m_locationconfig.getRootPath() + requestUri;

	return dirPath;
}
