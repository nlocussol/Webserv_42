#include "../inc/ErrorPage.hpp"
#include "../inc/webserv.hpp"
#include <fstream>
#include <string>
#include <iostream>

ErrorPage& ErrorPage::getInstance()
{
	static ErrorPage instance;
	return instance;
}

std::string ErrorPage::getConfPage(std::string file, int &code) {
	std::fstream errpage(file.c_str());
	if (!errpage) {
		cout << "ff" << endl;
		return NULL;
	}
	std::string line;
	std::string buff = "HTTP/1.1 " + itostr(code) + " Bad Request\r\n"
	"Content-type: text/html\r\n"
	"Connection: closed\r\n"
	"\r\n";
	while (getline (errpage, line)) {
		buff += line + "\r\n";
		line.clear();
	}
	return buff;
}

std::string ErrorPage::getErrorPage(int statusCode)
{
	switch (statusCode) {
		case 404:
			std::cerr << "Error 404: Client asked for non existing ressource\n";
			return _404Page;
			break;
		case 403:
			std::cerr << "Error 403: Client asked for forbidden ressource\n";
			return _403Page;
			break;
		case 405:
			std::cerr << "Error 405: Client sent a method not allowed\n";
			return _405Page;
			break;
		case 400:
			std::cerr << "Error 400: Client sent a bad request\n";
			return _400Page;
			break;
		case 507:
			std::cerr << "Error 507: Insufficient Storage\n";
			return _507Page;
			break;
		case 508:
			std::cerr << "Error 508: Loop detected\n";
			return _508Page;
			break;
		case 500:
			std::cerr << "Error 508: Internal Server Error\n";
			return _508Page;
			break;
		case 301:
			std::cerr << "Code 301: Permanent redirection\n";
			return _301Page;
			break;
	}
	std::string ff = "les problemes\n";
	return ff;
}

ErrorPage::ErrorPage()
{
}

std::string ErrorPage::_400Page = "HTTP/1.1 400 Bad Request\r\n"
"Content-type: text/html\r\n"
"Connection: closed\r\n"
"\r\n"
"<html>\r\n"
"<body>\r\n"
"<h1>Bad Request</h1>\r\n"
"<hr>\r\n"
"<p>The request sent is not valid.</p>\r\n"
"</body>\r\n"
"</html>\r\n\r\n";

std::string ErrorPage::_404Page = "HTTP/1.1 404 Not Found\r\n"
"Content-type: text/html\r\n"
"Connection: closed\r\n"
"\r\n"
"<html>\r\n"
"<body>\r\n"
"<h1>Not Found</h1>\r\n"
"<hr>\r\n"
"<p>The requested URL was not found on this server.</p>\r\n"
"</body>\r\n"
"</html>\r\n\r\n";

std::string ErrorPage::_403Page = "HTTP/1.1 403 Forbidden\r\n"
"Content-type: text/html\r\n"
"Connection: closed\r\n"
"\r\n"
"<html>\r\n"
"<body>\r\n"
"<h1>Forbidden</h1>\r\n"
"<hr>\r\n"
"<p>You don't have permission to access this URL on this server.</p>\r\n"
"</body>\r\n"
"</html>\r\n\r\n";

std::string ErrorPage::_405Page = "HTTP/1.1 405 Not Allowed\r\n"
"Content-type: text/html\r\n"
"Connection: closed\r\n"
"\r\n"
"<html>\r\n"
"<body>\r\n"
"<h1>Method Not Allowed</h1>\r\n"
"<hr>\r\n"
"<p>This error does not allow this method.</p>\r\n"
"</body>\r\n"
"</html>\r\n\r\n";

std::string ErrorPage::_500Page = "HTTP/1.1 500 Internal Server Error\r\n"
"Content-type: text/html\r\n"
"Connection: closed\r\n"
"\r\n"
"<html>\r\n"
"<body>\r\n"
"<h1>Internal Server Error</h1>\r\n"
"<hr>\r\n"
"<p>The server encountered an unexpected condition that prevented it from fulfilling the request.</p>\r\n"
"</body>\r\n"
"</html>\r\n\r\n";

std::string ErrorPage::_507Page = "HTTP/1.1 507 Insufficient Storage\r\n"
"Content-type: text/html\r\n"
"Connection: closed\r\n"
"\r\n"
"<html>\r\n"
"<body>\r\n"
"<h1>Insufficient Storage</h1>\r\n"
"<hr>\r\n"
"<p>The server cannot store the representation needed to successfully complete the request.</p>\r\n"
"</body>\r\n"
"</html>\r\n\r\n";

std::string ErrorPage::_508Page = "HTTP/1.1 508 Loop Detected\r\n"
"Content-type: text/html\r\n"
"Connection: closed\r\n"
"\r\n"
"<html>\r\n"
"<body>\r\n"
"<h1>Loop Detected</h1>\r\n"
"<hr>\r\n"
"<p>The server terminated an operation because it encountered an infinite loop while processing a request.</p>\r\n"
"</body>\r\n"
"</html>\r\n\r\n";

std::string ErrorPage::_301Page = "HTTP/1.1 301 Moved permanently\r\nLocation: ";
