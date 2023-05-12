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

Response::Response(block_serv serv)
{
	_contentType.first = "Content-Type: ";
	_contentLength.first = "Content-Length: ";
	_connection.first = "Connection: ";
	_connection.second = "keep-alive";
	_server = serv;
}

void Response::buildResponse(Request& request)
{
	if (request._statusCode == 301) {
		handleRedirection(request);
	}
	else if (request._statusCode == 200 || request._statusCode == 201) {
		switch (request._methodInt) {
			case GET_REQUEST:
				if (request._cgi) {
				 	handleGetCGI(request);
				} 
				else if (request._dirList)
					handleDirectoryListing(request._filePath);
				else {
					buildGetHeader(request._extension);
					buildGetBody(request._filePath);
				}
				break;
			case POST_REQUEST:
				buildPostHeader(request._requestSubType);
				return;
				break;
			case DELETE_REQUEST:
				break;
		}
		//do this in another function and probably need to rename functions
		buildCompleteResponse(request._statusCode);
	}
	else
		buildErrorResponse(request);
}

void Response::handleRedirection(Request& request)
{
	ErrorPage& errorPage = ErrorPage::getInstance();
	_completeResponse = errorPage.getErrorPage(request._statusCode);
	_completeResponse += request._filePath + _CRLF + _CRLF;
}

void Response::buildGetHeader(const std::string& extension)
{
	if (extension == "jpg") {
			_contentType.second = "image/jpg";
	}
	else if (extension == "jpeg") {
			_contentType.second = "image/jpeg";
	}
	else if (extension == "ico") {
			_contentType.second = "image/jpg";
	}
	else {
			_contentType.second = "text/html; charset=utf-8";
	}
}

bool Response::check_dir(string & filePath, block_serv & server) {
	DIR *dir = opendir(filePath.c_str());
	if (dir == NULL)
		return true;
	closedir(dir);
	MULTIMAP copy = find_location_path(filePath, server);
	MULTIMAP::iterator index = copy.find("index");
	if (index != copy.end()) {
		if (filePath[filePath.size() - 1] != '/')
			filePath += "/";
		filePath += index->second;
		return true;
	}
	return false;
}

void Response::buildGetBody(std::string& filePath)
{
	// probably need to test if every syscall worked
	// if (!check_dir(filePath, server)) {
	// 	cout << "on a un pb" << endl;
	// 	return ;
	// }
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

void Response::handleGetCGI(const Request& request)
{
	std::string body = request._cgiBody;

	_contentType.second = "text/html";
	_contentLength.second = body.length();
	_binaryData = body;	
}

void Response::handleDirectoryListing(const std::string& filePath)
{
	_contentType.second = "text/html";
	_contentLength.second = directory_listing(filePath, _server).length();
	_binaryData = directory_listing(filePath, _server);	
}

void Response::buildPostHeader(int requestSubType)
{
	(void) requestSubType;
	_completeResponse = "HTTP/1.1 303 See Other\r\nLocation: /index.html\r\nContent-Length: 0\r\n\r\n";

}

void Response::buildCompleteResponse(int statusCode)
{
	_completeResponse =
		_HTTPVersion + itostr(statusCode) + _CRLF 
		+ _contentType.first + _contentType.second + _CRLF 
		+ _contentLength.first + itostr(_contentLength.second) + _CRLF
		+ _connection.first + _connection.second + _CRLF
		+ _CRLF
		+ _binaryData;
}

// Check if there is a defined file for error page, else use default one
void Response::buildErrorResponse(Request & request)
{
	ErrorPage& errorPage = ErrorPage::getInstance();
	MULTIMAP copy = find_location_path(request._filePath, request._servers.v_serv[request._serverId]);
	MULTIMAP::iterator it = copy.find("errpage_" + itostr(request._statusCode));
	if (it != copy.end())
		_completeResponse = errorPage.getConfPage(copy.find("root")->second + it->second, request._statusCode);
	else
		_completeResponse = errorPage.getErrorPage(request._statusCode);
}

std::string& Response::getCompleteResponse(void)
{
	return _completeResponse;
}

std::string Response::_CRLF = "\r\n";
std::string Response::_HTTPVersion = "HTTP/1.1 ";

Response::~Response()
{
}
