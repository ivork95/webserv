#include "Request.hpp"
#include "Multiplexer.hpp"
#include "CGIPipeIn.hpp"
#include "CGIPipeOut.hpp"

int Request::uploadHandler(void)
{
    m_response.bodySet(Helper::fileToStr("./www" + m_methodPathVersion[1]));
    m_response.statusCodeSet(200);

	return 0;
}

int Request::getHandler(void)
{

    // Can't find an index file, check if directory listing
    if (m_response.pathGet().empty())
    {
        const std::string dirPath = directoryListingParse();
        if (dirPath.empty())
            throw StatusCodeException(500, "Error: directoryListingParse");
        if (!std::filesystem::is_directory(dirPath))
            throw StatusCodeException(404, "Error: no matching index file found");
        else
        {
            if (!m_locationconfig.getAutoIndex())
                throw StatusCodeException(403, "Forbidden: directory listing disabled");
            directoryListingBodySet(dirPath);

            return 0;
        }
    }
    m_response.bodySet(Helper::fileToStr(m_response.pathGet()));
    m_response.statusCodeSet(200);

	return 0;
}