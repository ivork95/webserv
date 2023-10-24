#include "Request.hpp"

// TODO fix this for absolute/relative path
static std::string generateHtmlString(const std::string &path)
{
    std::stringstream htmlStream;

    htmlStream << "<html><head><title>File and Directory List</title></head><body>\n";
    htmlStream << "<h1>Files and Directories in " << path << "</h1>\n";
    htmlStream << "<ul>\n";

    for (const auto &entry : std::filesystem::directory_iterator(path))
    {
        const std::string entryName = entry.path().filename().string();
        if (std::filesystem::is_directory(entry))
        {
            htmlStream << "<li><strong>Directory:</strong> <a href=\"" << entryName << "/\">" << entryName << "</a></li>\n";
        }
        else if (std::filesystem::is_regular_file(entry))
        {
            htmlStream << "<li><strong>File:</strong> " << entryName << " ";
            htmlStream << "<button onclick=\"deleteFile('" << entryName << "')\">Delete</button></li>\n";
        }
    }

    htmlStream << "</ul>\n";
    htmlStream << "<script>\n";
    htmlStream << "function deleteFile(fileName) {\n";
    htmlStream << "    if (confirm('Are you sure you want to delete ' + fileName + '?')) {\n";
    htmlStream << "        var xhr = new XMLHttpRequest();\n";
    htmlStream << "        xhr.open('DELETE', fileName, true);\n";
    htmlStream << "        xhr.onreadystatechange = function() {\n";
    htmlStream << "            if (xhr.readyState === 4 && xhr.status === 200) {\n";
    htmlStream << "                alert('File ' + fileName + ' deleted.');\n";
    htmlStream << "                location.reload();\n";
    htmlStream << "            }\n";
    htmlStream << "        };\n";
    htmlStream << "        xhr.send();\n";
    htmlStream << "    }\n";
    htmlStream << "}\n";
    htmlStream << "</script>\n";
    htmlStream << "</body></html>\n";

    return htmlStream.str();
}

static std::string directoryListingGenerate(const std::string &dirPath) {
    std::string listing = "<html><body><h1>" + dirPath + "</h1><ul>";

    for (const auto &entry : std::filesystem::directory_iterator(dirPath))
    {
        const std::string entryName = entry.path().filename().string();
        const std::string entryPath = entry.path().string();

        if (std::filesystem::is_directory(entryPath))
        {
            listing += "<li><a href='" + entryName + "/'>" + entryName + "/</a></li>";
        }
        else
        {
			listing += "<li>" + entryName + "</li>";
        }
    }

    listing += "</ul></body></html>";

    return listing;
}

void	Request::directoryListingBodySet(const std::string &dirPath) {
	// const std::string directoryListing = directoryListingGenerate(dirPath);
	const std::string directoryListing = generateHtmlString(dirPath);
	if (directoryListing.empty())
		throw StatusCodeException(500, "Error: directoryListingBodySet");

	// m_response.bodySet(directoryListing); // to change I guess
	m_response.m_body = directoryListing;
	m_response.m_statusCode = 200;
	m_response.m_headers.insert({"Content-Type", "text/html"});
}

std::string	Request::directoryListingParse(void) {
	std::string requestUri = m_methodPathVersion[1];
	std::string dirPath{};

	// Remove leading '/' before appending root
	if (requestUri[0] == '/')
		requestUri = requestUri.substr(1, requestUri.size());

	dirPath = m_locationconfig.getRootPath() + requestUri;

	return dirPath;
}
