#include "../inc/Response.hpp"
#include "../inc/Request.hpp"
#include "../inc/Server.hpp"
#include "../inc/webserv.hpp"
#include <cstdio>
#include <cstring>
#include <iostream>
#include <fstream>
#include <ios>
#include <limits>
#include <sstream>
#include <streambuf>
#include <utility>

std::string Response::_CRLF = "\r\n";

std::string Response::_404Page = "HTTP/1.1 404 Not Found\r\n"
"Content-type: text/html\r\n"
"\r\n"
"<html>\r\n"
"<body>\r\n"
"<h1>Not Found</h1>\r\n"
"<p>The requested URL was not found on this server.</p>\r\n"
"</body>\r\n"
"</html>\r\n\r\n";

std::string Response::_403Page = "HTTP/1.1 403 Forbidden\r\n"
"Content-type: text/html\r\n"
"\r\n"
"<html>\r\n"
"<body>\r\n"
"<h1>Forbidden</h1>\r\n"
"<p>You don't have permission to access this URL on this server.</p>\r\n"
"</body>\r\n"
"</html>\r\n\r\n";

std::string Response::_405Page = "HTTP/1.1 405 Not Allowed\r\n"
"Content-type: text/html\r\n"
"\r\n"
"<html>\r\n"
"<body>\r\n"
"<h1>Method Not Allowed</h1>\r\n"
"<p>This error does not allow this method.</p>\r\n"
"</body>\r\n"
"</html>\r\n\r\n";

Response::Response()
{
	_statusCode = 0;
	_contentType.first = "Content-Type: ";
	_contentLength.first = "Content-Length: ";
	_connection.first = "Connection: ";
	_connection.second = "keep_alive";
}

Response::~Response()
{
}

void Response::buildResponse(const Request& request, const std::string& filePath)
{
	if (request._statusCode == 200 || request._statusCode == 201) {
		switch (request._requestType) {
			case GET_REQUEST:
				buildGetHeader(request._requestSubType);
				buildGetBody(filePath);
				break;
			case POST_REQUEST:
				buildPostHeader(request._requestSubType);
				break;
			case DELETE_REQUEST:
				break;
			case DIR_LISTING:
				_contentType.second = "text/html";
				_contentLength.second = directory_listing(filePath).length();
				_binaryData = directory_listing(filePath);
				break;
			default:
				break;
		}
		//do this in another function and probably need to rename functions
		buildCompleteResponse(request._statusCode);
	}
	else if (request._statusCode == 7)
	{
	
		buildCompleteResponse(request._statusCode);
	}
	else
		buildErrorResponse(request._statusCode);
}

void Response::buildGetHeader(int requestSubType)
{
	switch (requestSubType) {
		case TEXT:
			_contentType.second = "text/html";
			break;
		case IMAGE:
			_contentType.second = "image/*";
			break;
		case VIDEO:
			break;
		default:
			break;
	}
}

void Response::buildGetBody(const std::string& filePath)
{
	//probably need to test if every syscall worked
	std::ifstream file(filePath.c_str(), std::ios::in | std::ios::binary);
	if (file) {
		//Find file size and append it to buffer
		file.seekg(0, std::ios::end);
		_contentLength.second = file.tellg();
		file.seekg(0, std::ios::beg);

		const char* fileData = new char [_contentLength.second];
		file.read((char *)fileData, _contentLength.second);
		file.close();

		//Write binary data of file in _binaryData
		std::ostringstream fileSs;
		fileSs.write(fileData, _contentLength.second);
		_binaryData = fileSs.str();
		delete [] fileData;
	}
	else 
		std::cerr << "Error: Failed opening file to get binary data\n ";
}

void Response::buildPostHeader(int requestSubType)
{
	//Idk ??
}

void Response::buildCompleteResponse(int statusCode)
{
	_completeResponse =
		"HTTP/1.1 " + itostr(statusCode) + _CRLF 
		+ _contentType.first + _contentType.second + _CRLF 
		+ _contentLength.first + itostr(_contentLength.second) + _CRLF
		+ _connection.first + _connection.second + _CRLF
		+ _CRLF
		+ _binaryData;
}

void Response::buildErrorResponse(int statusCode)
{
	switch (statusCode) {
		case 404:
			std::cerr << "Error 404: Client asked for non existing ressource\n";
			_completeResponse = _404Page;
			break;
		case 403:
			std::cerr << "Error 403: Client asked for forbidden ressource\n";
			_completeResponse = _403Page;
			break;
		case 405:
			std::cerr << "Error 405: Client asked for forbidden ressource\n";
			_completeResponse = _405Page;
			break;
	}
}

void Response::setStatusCode(int statusCode)
{
	_statusCode = statusCode;
}

int Response::getStatusCode(void)
{
	return _statusCode;
}

std::string& Response::getCompleteResponse(void)
{
	return _completeResponse;
}


//Probably move this function somewhere else
std::string Response::itostr(int i)
{
	std::stringstream ss;
	ss << i;
	return ss.str();
}
