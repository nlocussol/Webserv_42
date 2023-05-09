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
