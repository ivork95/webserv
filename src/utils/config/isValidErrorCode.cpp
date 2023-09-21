
#include "UtilsConfig.hpp"

/**
 * TODO valid error codes? reserved values?
 * 	1xx Informational: request was received and understood but that further action is required by the client.
 * 	2xx Success: request was received, understood, and accepted successfully.
 * 	3xx Redirection: further action needs to be taken to fulfill the request, often involving redirection.
 * 	4xx Client Errors: issue with the client's request, such as a malformed request or authentication failure.
 * 	5xx Server Errors: issue on the server while processing the request.
*/
bool	isValidErrorCode(const std::string &str) {
	if (str.size() != 3)
		return false;
	if (!isNumber(str))
		return false;
	return true;
}
