#include "Request.hpp"
#include "Multiplexer.hpp"
#include "CGIPipeIn.hpp"
#include "CGIPipeOut.hpp"

static void deleteFile(const std::string &filePath)
{
    std::cout << "DELETE delete file handler\n";

    std::error_code ec{};

    if (!std::filesystem::remove(filePath, ec))
    {
        if (ec == std::errc::no_such_file_or_directory)
            throw StatusCodeException(404, "Error: File not found for DELETE request");
        else
            throw StatusCodeException(500, "Error: Failed to delete file");
    }
}

std::string Request::buildDeleteFilePath(void)
{
    std::cout << "DELETE build path handler\n";

    std::filesystem::path rootPath(m_locationconfig.getRootPath());
    std::filesystem::path rootParentPath = rootPath.parent_path();
    std::string fullPath = rootParentPath.string() + m_methodPathVersion[1];

    return fullPath;
}

int Request::deleteHandler(void)
{
    std::cout << "DELETE handler\n";

    std::filesystem::path requestPath(m_methodPathVersion[1]);
    std::filesystem::path requestParentPath = requestPath.remove_filename();

    updatedLocationConfigSet(requestParentPath);
    isMethodAllowed();
    const std::string filePath = buildDeleteFilePath();
    deleteFile(filePath);
    m_body = "Success: File deleted";
    m_response.m_statusCode = 200;

    return 0;
}