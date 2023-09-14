#ifndef EXCEPTIONS_HPP
# define EXCEPTIONS_HPP

# include <exception>
# include <string>

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
			: InvalidTokenException("Port number: " + input) {}
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
			: InvalidTokenException("Client max body size: " + input) {}
};

class UriException : public InvalidTokenException {
	public:
		UriException(const std::string &input)
			: InvalidTokenException("Uri: " + input) {}
};

class RootException : public InvalidTokenException {
	public:
		RootException(const std::string &input)
			: InvalidTokenException("Root: " + input) {}
};

class AutoIndexException : public InvalidTokenException {
	public:
		AutoIndexException(const std::string &input)
			: InvalidTokenException("Auto index: " + input) {}
};

class IndexException : public InvalidTokenException {
	public:
		IndexException(const std::string &input)
			: InvalidTokenException("Index: " + input) {}
};

class CgiException : public InvalidTokenException {
	public:
		CgiException(const std::string &input)
			: InvalidTokenException("Cgi: " + input) {}
};

class CgiExtensionException : public InvalidTokenException {
	public:
		CgiExtensionException(const std::string &input)
			: InvalidTokenException("Cgi extension: " + input) {}
};

class HttpMethodException : public InvalidTokenException {
	public:
		HttpMethodException(const std::string &input)
			: InvalidTokenException("Http method: " + input) {}
};

class PathException : public InvalidTokenException {
	public:
		PathException(const std::string &input)
			: InvalidTokenException("Path: " + input) {}
};

class ConversionUnitException : public InvalidTokenException {
	public:
		ConversionUnitException(const std::string &input)
			: InvalidTokenException("Conversion unit: " + input) {}
};

#endif
