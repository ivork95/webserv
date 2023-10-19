#include "Request.hpp"

void	Request::chunkBodySet() {
	std::string finalChunkBody{};
	std::string tmp{};
	for (size_t n = 0; n < m_chunkLine.size(); n++) {
		tmp = m_chunkLine[n].substr(0, m_chunkLine[n].size() - 1); // to remove trailing \n
		finalChunkBody.append(tmp);
	}
	m_chunkBody = finalChunkBody;
}

// ! can be static
void	Request::chunkBodyParse(size_t nbChunks, std::vector<size_t> chunkLength, std::vector<std::string> chunkLine) {
	// Error check
	if (chunkLine.empty())
		throw StatusCodeException(404, "Error: Empty chunk request");

	// Compare chunk length to actual chunk length
	for (size_t n = 0; n < nbChunks; n++) {
		if (chunkLength[n] != chunkLine[n].size() - 1)
				throw StatusCodeException(400, "Warning: chunk size is different from the chunk line length");
	}
}

void	Request::chunkBodyTokenize(void) {
	std::istringstream			iss(m_rawChunkBody);
	std::string					token{};
	std::vector<std::string> 	chunkLine{};
	std::vector<size_t> 		chunkLength{};
	size_t 						nbLines = 0;

	// Split lines and save chunk len and actual chunk separately
	while (std::getline(iss, token)) {
		if (token.empty())
			break ;
		if (nbLines % 2 == 0) {
			int intLen = Helper::hexToInt(token);
			chunkLength.push_back(intLen);
		} else {
			chunkLine.push_back(token);
		}
		nbLines++;
	}

	size_t 						nbChunks = (nbLines - 1) / 2; // since 1 chunk = chunk size + actual chunk
	
	chunkBodyParse(nbChunks, chunkLength, chunkLine);
	m_chunkLine = chunkLine;
}

void	Request::chunkBodyExtract(void) {
	const std::string headersEnd = "\r\n\r\n";
	const std::string chunkEnd = "\r\n0\r\n\r\n";
	size_t requestHeadersEndPos = m_rawMessage.find(headersEnd);
	size_t generalHeadersEndPos = m_rawMessage.find(chunkEnd);

	m_rawChunkBody = m_rawMessage.substr(requestHeadersEndPos + 4, generalHeadersEndPos - requestHeadersEndPos);
}

void	Request::chunkHeadersParse(void) {
	for (auto header: m_requestHeaders) {
		if (header.first == "Transfer-Encoding") {
			if (header.second != "chunked") {
				throw StatusCodeException(404, "Error: Invalid Transfer-Encoding form");
			}
		}
	}
}