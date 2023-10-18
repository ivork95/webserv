#include "HttpRequest.hpp"

void	HttpRequest::chunkHeaderReplace() {
	std::string key = "Content-Length";
	std::string value = std::to_string(m_totalChunkLength);
	
	auto it = m_requestHeaders.find("Transfer-Encoding");
	if (it != m_requestHeaders.end()) {
		// Key found, insert a new key-value pair and remove the old one
		m_requestHeaders.insert(std::make_pair(key, value));
		m_requestHeaders.erase(it);
	} else {
		// Key not found, insert a new key-value pair
		m_requestHeaders.insert(std::make_pair(key, value));
	}
}

void	HttpRequest::chunkBodySet() {
	std::string finalChunkBody{};
	std::string tmp{};
	for (size_t n = 0; n < m_chunkLine.size(); n++) {
		tmp = m_chunkLine[n].substr(0, m_chunkLine[n].size() - 1); // to remove trailing \n
		finalChunkBody.append(tmp);
	}
	m_chunkBody = finalChunkBody;
}

static void	chunkBodyParse(size_t nbLines, std::vector<size_t> chunkLength, std::vector<std::string> chunkLine) {
	if (chunkLine.empty())
		throw StatusCodeException(400, "Error: empty chunk request");
	
	if (nbLines % 2 != 1)
		throw StatusCodeException(400, "Error: invalid chunk body");
	
	// 1 chunk = chunk size + actual chunk
	size_t	nbChunks = (nbLines - 1) / 2;

	for (size_t n = 0; n < nbChunks; n++) {
		if (chunkLength[n] != chunkLine[n].size() - 1)
				throw StatusCodeException(400, "Warning: chunk size is different from the chunk line length");
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

	chunkBodyParse(nbLines, chunkLength, chunkLine);
	m_chunkLine = chunkLine;
}

// TODO probably needs tweaking
void	HttpRequest::chunkBodyExtract(void) {
	const std::string headersEnd = "\r\n\r\n";
	const std::string chunkEnd = "\r\n0\r\n\r\n";
	size_t requestHeadersEndPos = m_rawMessage.find(headersEnd);
	size_t generalHeadersEndPos = m_rawMessage.find(chunkEnd);

	m_rawChunkBody = m_rawMessage.substr(requestHeadersEndPos + 4, generalHeadersEndPos - requestHeadersEndPos);
	// spdlog::warn("m_rawMessage = {}", m_rawMessage);
	// spdlog::warn("m_rawChunkBody = {}", m_rawChunkBody);
	// spdlog::warn("requestHeadersEndPos = {}", requestHeadersEndPos);
	// spdlog::warn("generalHeadersEndPos = {}", generalHeadersEndPos);
}

void	HttpRequest::chunkHeadersParse(void) {
	auto it = m_requestHeaders.find("Transfer-Encoding");
	if (it != m_requestHeaders.end()) {
		// Found
		if (it->second != "chunked") {
			throw StatusCodeException(400, "Error: invalid Transfer-Encoding form");
		}
	} else {
		// Not found
		throw StatusCodeException(400, "Error: could not find Transfer-Encoding");
	}
}