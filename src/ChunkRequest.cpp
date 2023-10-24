#include "HttpRequest.hpp"

void	HttpRequest::chunkHeaderReplace() {
	std::string key = "Content-Length";
	std::string value = std::to_string(m_totalChunkLength);
	
	auto it = m_requestHeaders.find("Transfer-Encoding");
	if (it != m_requestHeaders.end()) {
		m_requestHeaders.insert(std::make_pair(key, value));
		m_requestHeaders.erase(it);
	} else {
		m_requestHeaders.insert(std::make_pair(key, value));
	}
}

void	HttpRequest::chunkBodySet() {
	std::string finalChunkBody{};
	std::string tmp{};

	for (size_t n = 0; n < m_chunkLine.size(); n++) {
		try {
			tmp = m_chunkLine[n].substr(0, m_chunkLine[n].size() - 1); // to remove trailing \n
			finalChunkBody.append(tmp);
		} catch (...) {
			throw StatusCodeException(500, "Error: chunkBodySet substr");
		}
	}

	if (finalChunkBody.empty()) {
		throw StatusCodeException(500, "Error: chunkBodySet finalChunkBody");
	}

	m_chunkBody = finalChunkBody;
}

static void	chunkBodyParse(size_t nbLines, std::vector<size_t> chunkLength, std::vector<std::string> chunkLine) {
	if (chunkLine.empty())
		throw StatusCodeException(500, "Error: empty chunk request");
	
	if (nbLines % 2 != 1)
		throw StatusCodeException(500, "Error: invalid chunk body");
	
	// 1 chunk = chunk size + actual chunk
	size_t	nbChunks = (nbLines - 1) / 2;

	for (size_t n = 0; n < nbChunks; n++) {
		if (chunkLength[n] != chunkLine[n].size() - 1)
				throw StatusCodeException(500, "Warning: chunk size is different from the chunk line length");
	}
}

void	HttpRequest::chunkBodyTokenize(void) {
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
			m_totalChunkLength += intLen;
		} else {
			chunkLine.push_back(token);
		}
		nbLines++;
	}

	if (chunkLine.empty() || chunkLength.empty()) {
		throw StatusCodeException(500, "Error: chunkBodyTokenize");
	}

	chunkBodyParse(nbLines, chunkLength, chunkLine);
	m_chunkLine = chunkLine;
}

void	HttpRequest::chunkBodyExtract(void) {
	const std::string headersEnd = "\r\n\r\n";
	size_t headersEndPos = m_rawMessage.find(headersEnd);
	const std::string chunkEnd = "\r\n0\r\n\r\n";
	size_t chunkEndPos = m_rawMessage.find(chunkEnd);

	try {
		m_rawChunkBody = m_rawMessage.substr(headersEndPos + 4, chunkEndPos - headersEndPos); // + 4 to remove CRLF (Carriage Return - Line Feed)
	} catch (...) {
		throw StatusCodeException(500, "Error: chunkBodyExtract substr");
	}
}

void	HttpRequest::chunkHeadersParse(void) {
	auto it = m_requestHeaders.find("Transfer-Encoding");
	if (it != m_requestHeaders.end()) {
		if (it->second != "chunked") {
			throw StatusCodeException(500, "Error: invalid Transfer-Encoding form");
		}
	} else {
		throw StatusCodeException(500, "Error: could not find Transfer-Encoding");
	}
}
