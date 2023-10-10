#ifndef EXCEPTIONS_HPP
# define EXCEPTIONS_HPP

# include <exception>
# include <string>

/**
 * TOKEN EXCEPTIONS
*/
class InvalidTokenException : public std::exception {
	public:
		InvalidTokenException(const std::string &derivedMsg)
			: derivedErrorMessage(derivedMsg) {
			fullErrorMessage = "Invalid token: " + derivedErrorMessage;
		}

		const char* what() const noexcept override {
			return fullErrorMessage.c_str();
		}

	private:
		std::string			derivedErrorMessage;
		std::string			fullErrorMessage;
};

class WordException : public InvalidTokenException {
	public:
		WordException()
			: InvalidTokenException("Expected WORD") {}
};

class PortNumberException : public InvalidTokenException {
	public:
		PortNumberException(const std::string &input)
			: InvalidTokenException("Port number: " + input + " (only from 0 to 65535)") {}
};

class ServerNameException : public InvalidTokenException {
	public:
		ServerNameException(const std::string &input)
			: InvalidTokenException("Server name: " + input) {}
};

class ErrorCodeException : public InvalidTokenException {
	public:
		ErrorCodeException(const std::string &input)
			: InvalidTokenException("Error code: " + input) {}
};

class ClientMaxBodySizeException : public InvalidTokenException {
	public:
		ClientMaxBodySizeException(const std::string &input)
			: InvalidTokenException("Client max body size: " + input + " (only positive numbers)") {}
};

class UriException : public InvalidTokenException {
	public:
		UriException(const std::string &input)
			: InvalidTokenException("URI: " + input) {}
};

class RootException : public InvalidTokenException {
	public:
		RootException(const std::string &input)
			: InvalidTokenException("Root: " + input) {}
};

class AutoIndexException : public InvalidTokenException {
	public:
		AutoIndexException(const std::string &input)
			: InvalidTokenException("Auto index: " + input + " (only on or off)") {}
};

class IndexException : public InvalidTokenException {
	public:
		IndexException(const std::string &input)
			: InvalidTokenException("Index: " + input) {}
};

class CgiException : public InvalidTokenException {
	public:
		CgiException(const std::string &input)
			: InvalidTokenException("CGI: " + input) {}
};

class CgiExtensionException : public InvalidTokenException {
	public:
		CgiExtensionException(const std::string &input)
			: InvalidTokenException("CGI extension: " + input + " (only .py, .php or .c)") {}
};

class HttpMethodException : public InvalidTokenException {
	public:
		HttpMethodException(const std::string &input)
			: InvalidTokenException("HTTP method: " + input + " (only GET, POST or DELETE)") {}
};

class PathException : public InvalidTokenException {
	public:
		PathException(const std::string &input)
			: InvalidTokenException("Path: " + input) {}
};

class ConversionUnitException : public InvalidTokenException {
	public:
		ConversionUnitException(const std::string &input)
			: InvalidTokenException("Conversion unit: " + input + " (only k/K, m/M or g/G)") {}
};

/**
 * DIRECTIVE EXCEPTIONS
*/
class InvalidDirectiveException : public std::exception {
	public:
		InvalidDirectiveException(const std::string &derivedMsg)
			: derivedErrorMessage(derivedMsg) {
			fullErrorMessage = "Invalid directive: " + derivedErrorMessage;
		}

		const char* what() const noexcept override {
			return fullErrorMessage.c_str();
		}

	private:
		std::string			derivedErrorMessage;
		std::string			fullErrorMessage;
};

class AlreadySetException : public InvalidDirectiveException {
	public:
		AlreadySetException(const std::string &input)
			: InvalidDirectiveException("Already set: " + input) {}
};

class DuplicateRequestUriException : public InvalidDirectiveException {
	public:
		DuplicateRequestUriException(const std::string &input)
			: InvalidDirectiveException("Duplicate request URI: " + input) {}
};

class DuplicateServerNameException : public InvalidDirectiveException {
	public:
		DuplicateServerNameException(const std::string &input)
			: InvalidDirectiveException("Duplicate server name: " + input) {}
};

#endif
