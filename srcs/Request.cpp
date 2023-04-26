#include "../inc/Request.hpp"
#include "../inc/Server.hpp"
#include <cstdio>
#include <cstring>
#include <iostream>
#include <fstream>
#include <ios>
#include <limits>
#include <sstream>

std::string Request::_404 = "HTTP/1.1 404 Not Found\r\n"
      "Content-type: text/html\r\n"
      "\r\n"
      "<html>\r\n"
      "<body>\r\n"
      "<h1>Not Found</h1>\r\n"
      "<p>The requested URL was not found on this server.</p>\r\n"
      "</body>\r\n"
      "</html>\r\n";

Request::Request()
{
}

Request::~Request()
{
}

Request::Request(const Request& other)
{
}

Request& Request::operator=(const Request& other)
{
	return *this;
}

std::string& Request::getBuffer(void)
{
	return _buffer;
}

Request::Request(int requestType, int requestSubType, int statusCode, std::string filePath)
{
	// std::cout << statusCode << '\n';
	if (statusCode == 200) {
		switch (requestType) {
			case GET_REQUEST:
				//Add SubFunctions
				switch (requestSubType) {
					case TEXT:
						_buffer = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: keep-alive\r\nContent-Length: ";
						buildGetResponse(filePath);
						break;
					case IMAGE:
						_buffer = "HTTP/1.1 200 OK\r\nContent-Type: image/*\r\nConnection: keep-alive\r\nContent-Length: ";
						buildGetResponse(filePath);
						break;
					default :
						break;
				}
				break;
			case POST_REQUEST:
				break;
			case DELETE_REQUEST:
				break;
		}
	}
	handleError(statusCode);
}

void Request::buildGetResponse(std::string filePath)
{
	std::ifstream file(filePath.c_str(), std::ios::in | std::ios::binary);

	if (file) {
		//Find file size and append it to buffer
		file.seekg(0, std::ios::end);
		int fileSize = file.tellg();
		file.seekg(0, std::ios::beg);

		const char* fileData = new char [fileSize];
		file.read((char *)fileData, fileSize);
		file.close();

		//Append file data to buffer
		std::ostringstream fileSs;
		fileSs << fileSize;
		fileSs << "\r\n\r\n";
		fileSs.write(fileData, fileSize);
		_buffer += fileSs.str();
		delete [] fileData;
	}
}

void Request::handleError(int statusCode)
{
	switch (statusCode) {
		case 404:
			std::cerr << "404 Not Found sent\n";
			_buffer = _404;
			break;
	}
}
