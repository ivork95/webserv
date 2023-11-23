#include "Request.hpp"
#include "Multiplexer.hpp"
#include "CGIPipeIn.hpp"
#include "CGIPipeOut.hpp"

static bool isValidPath(const std::string &str)
{
	if (str.empty())
		return false;
	if (!std::filesystem::exists(str))
		return false;
	if (!std::filesystem::is_regular_file(str))
		return false;
	return true;
}

static void deleteFile(const std::string &filePath)
{

	if (!isValidPath(filePath))
	{
		throw StatusCodeException(404, "Error: File not found for DELETE request");
	}

    if (!std::filesystem::remove(filePath))
    {
		throw StatusCodeException(500, "Error: Failed to delete file");
    }
}

std::string Request::buildDeleteFilePath(void)
{

    std::filesystem::path rootPath(m_locationconfig.getRootPath());
    std::filesystem::path rootParentPath = rootPath.parent_path();
    std::string fullPath = rootParentPath.string() + m_methodPathVersion[1];

    return fullPath;
}

int Request::deleteHandler(void)
{

    std::filesystem::path requestPath(m_methodPathVersion[1]);
    std::filesystem::path requestParentPath = requestPath.remove_filename();

    locationConfigSet(requestParentPath);
    isMethodAllowed();
    const std::string filePath = buildDeleteFilePath();
    deleteFile(filePath);
    m_response.statusCodeSet(204);

    return 0;
}