#include "Request.hpp"
#include "Server.hpp"
#include <cstdio>
#include <cstring>
#include <iostream>
#include <fstream>
#include <ios>
#include <limits>
#include <sstream>

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

void Request::buildTextHTMLRequest(std::string filePath)
{
	//Find file size
	std::ifstream file;
	file.open(filePath.c_str(), std::ios::in|std::ios::binary);
	file.ignore( std::numeric_limits<std::streamsize>::max() );
	std::streamsize length = file.gcount();
	file.clear();
	file.seekg(0, std::ios_base::beg);
	file.close();
	//Append file size to reponse's header
	std::stringstream nb;
	nb << length;
	_buffer += nb.str();
	_buffer += "\r\n\r\n";
	//Append binary data to reponse body
	char buff[BUFFER_SIZE];
	std::memset(buff, 0, BUFFER_SIZE);
	FILE *fileF = std::fopen(filePath.c_str(), "rb");
	int byte_r = 1;
	while (byte_r > 0) {
		std::fread(buff, BUFFER_SIZE - 1, 1, fileF);
		_buffer += buff;
	}
	fclose(fileF);
}

Request::Request(int type, std::string filePath)
{
	// std::cout << filePath << "\n\n";
	// switch (type) {
	// 	case TEXT_HTML:
			std::cout << "test";
			_buffer = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: keep-alive\r\nContent-Length: ";
			buildTextHTMLRequest(filePath);
			// break;
		// case IMAGE:
		// 	_buffer = "HTTP/1.1 200 OK\r\nContent-Type: image/*\r\nConnection: keep-alive\r\nContent-Length: ";
		// 	buildTextHTMLRequest("img.jpg");
		// 	break;
	// }
}
