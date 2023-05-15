#include "../inc/ErrorPage.hpp"
#include "../inc/webserv.hpp"
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>

ErrorPage& ErrorPage::getInstance()
{
	static ErrorPage instance;
	return instance;
}

std::string ErrorPage::getConfPage(std::string file, int &code) {
	std::fstream errpage(file.c_str());
	if (!errpage) {
		std::cerr << "ff" << endl;
		return NULL;
	}
	std::string body;
	std::string line;
	while (getline (errpage, line)) {
		body += line + "\r\n";
		line.clear();
	}
	std::string buff = "HTTP/1.1 " + itostr(code) + " Bad Request\r\n"
	"Content-type: text/html\r\n"
	"Connection: closed\r\n"
	"Content-Length: ";
	buff += bodySize(body);
	buff += "\r\n\r\n";
	buff += body;
	return buff;
}

std::string ErrorPage::getErrorPage(int statusCode)
{
	switch (statusCode) {
		case 404:
			std::cerr << "Error 404: Client asked for non existing ressource\n";
			return _404Header + bodySize(_404Body) + _404Body;
		case 403:
			std::cerr << "Error 403: Client asked for forbidden ressource\n";
			return _403Header + bodySize(_403Body) + _403Body;
		case 405:
			std::cerr << "Error 405: Client sent a method not allowed\n";
			return _405Header + bodySize(_405Body) + _405Body;
		case 400:
			std::cerr << "Error 400: Client sent a bad request\n";
			return _400Header + bodySize(_400Body) + _400Body;
		case 507:
			std::cerr << "Error 507: Insufficient Storage\n";
			return _507Header + bodySize(_507Body) + _507Body;
		case 508:
			std::cerr << "Error 508: Loop detected\n";
			return _508Header + bodySize(_508Body) + _508Body;
		case 500:
			std::cerr << "Error 500: Internal Server Error\n";
			return _500Header + bodySize(_500Body) + _500Body;
		case 301:
			std::cerr << "Code 301: Permanent redirection\n";
			return _301Header;
		case 413:
			std::cerr << "Error 414: Too long request\n";
			return _414Header + bodySize(_414Body) + _414Body;
		case 100:
			std::cerr << "Code 101: Continue\n";
			return _100Header;
		default:
			std::cerr << "Error 666: Error not handled\n";
			return "Error 666: You discovered an unhandled error gg";
	}
}

std::string ErrorPage::bodySize(const std::string& body)
{
	size_t bodySize = 0;
	for (size_t i = 0 ; body[i] ; i++) {
		if (!(body[i] == '\r' && body[i + 1] == '\n'))
			bodySize++;
		else
			i++;
	}
	std::stringstream ss;
	ss << bodySize;
	return ss.str();
}

ErrorPage::ErrorPage()
{
}

std::string ErrorPage::_400Header = "HTTP/1.1 400 Bad Request\r\n"
"Content-type: text/html\r\n"
"Connection: closed\r\n"
"Content-Length: ";

std::string ErrorPage::_400Body = "\r\n\r\n"
"<html>\r\n"
"<body>\r\n"
"<h1>Bad Request</h1>\r\n"
"<hr>\r\n"
"<p>The request sent is not valid.</p>\r\n"
"</body>\r\n"
"</html>";

std::string ErrorPage::_404Header = "HTTP/1.1 404 Not Found\r\n"
"Content-type: text/html\r\n"
"Connection: closed\r\n"
"Content-Length: ";

std::string ErrorPage::_404Body = "\r\n\r\n"
"<html>\r\n"
"<body>\r\n"
"<h1>Not Found</h1>\r\n"
"<hr>\r\n"
"<p>The requested URL was not found on this server.</p>\r\n"
"</body>\r\n"
"</html>";

std::string ErrorPage::_403Header = "HTTP/1.1 403 Forbidden\r\n"
"Content-type: text/html\r\n"
"Connection: closed\r\n"
"Content-Length: ";

std::string ErrorPage::_403Body = "\r\n\r\n"
"<html>\r\n"
"<body>\r\n"
"<h1>Forbidden</h1>\r\n"
"<hr>\r\n"
"<p>You don't have permission to access this URL on this server.</p>\r\n"
"</body>\r\n"
"</html>";

std::string ErrorPage::_405Header = "HTTP/1.1 405 Not Allowed\r\n"
"Content-type: text/html\r\n"
"Connection: closed\r\n"
"Content-Length: ";

std::string ErrorPage::_405Body = "\r\n\r\n"
"<html>\r\n"
"<body>\r\n"
"<h1>Method Not Allowed</h1>\r\n"
"<hr>\r\n"
"<p>This error does not allow this method.</p>\r\n"
"</body>\r\n"
"</html>";

std::string ErrorPage::_500Header = "HTTP/1.1 500 Internal Server Error\r\n"
"Content-type: text/html\r\n"
"Connection: closed\r\n"
"Content-Length: ";

std::string ErrorPage::_500Body = "\r\n\r\n"
"<html>\r\n"
"<body>\r\n"
"<h1>Internal Server Error</h1>\r\n"
"<hr>\r\n"
"<p>The server encountered an unexpected condition that prevented it from fulfilling the request.</p>\r\n"
"</body>\r\n"
"</html>";

std::string ErrorPage::_507Header = "HTTP/1.1 507 Insufficient Storage\r\n"
"Content-type: text/html\r\n"
"Connection: closed\r\n"
"Content-Length: ";

std::string ErrorPage::_507Body = "\r\n\r\n"
"<html>\r\n"
"<body>\r\n"
"<h1>Insufficient Storage</h1>\r\n"
"<hr>\r\n"
"<p>The server cannot store the representation needed to successfully complete the request.</p>\r\n"
"</body>\r\n"
"</html>";

std::string ErrorPage::_508Header = "HTTP/1.1 508 Loop Detected\r\n"
"Content-type: text/html\r\n"
"Connection: closed\r\n"
"Content-Length: ";

std::string ErrorPage::_508Body = "\r\n\r\n"
"<html>\r\n"
"<body>\r\n"
"<h1>Loop Detected</h1>\r\n"
"<hr>\r\n"
"<p>The server terminated an operation because it encountered an infinite loop while processing a request.</p>\r\n"
"</body>\r\n"
"</html>";

std::string ErrorPage::_414Header = "HTTP/1.1 414 URI Too Long\r\n"
"Content-type: text/html\r\n"
"Connection: closed\r\n"
"Content-Length: ";

std::string ErrorPage::_414Body = "\r\n\r\n"
"<html>\r\n"
"<body>\r\n"
"<h1>URI Too Long</h1>\r\n"
"<hr>\r\n"
"<p>URI provided by the client is too long.</p>\r\n"
"</body>\r\n"
"</html>";

std::string ErrorPage::_301Header = "HTTP/1.1 301 Moved permanently\r\n"
"Content-Length: 0\r\n"
"Location: ";

std::string ErrorPage::_100Header = "HTTP/1.1 100 Continue\r\n";
