#include "Request.hpp"
#include <cstdio>
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
	// _buffer = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: keep-alive\r\nContent-Length: ";
	std::ifstream file;

	file.open(filePath.c_str(), std::ios::in|std::ios::binary);
	file.ignore( std::numeric_limits<std::streamsize>::max() );
	std::streamsize length = file.gcount();
	file.clear();
	file.seekg(0, std::ios_base::beg);
	file.close();

	std::stringstream nb;
	nb << length;
	_buffer += nb.str();
	_buffer += "\r\n\r\n";

	char buff[256];
	FILE *test = fopen("index.html", "rb");
	while (!feof(test)) {
		fread(buff, 255, 1, test);
		_buffer += buff;
	}
	// std::cout << _buffer << std::endl;
	fclose(test);
}

Request::Request(int type)
{
	switch (type) {
		case TEXT_HTML:
			_buffer = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: keep-alive\r\nContent-Length: ";
			buildTextHTMLRequest("index.html");
			break;
		case 2:
			std::cout<< "test";
			_buffer = "HTTP/1.1 200 OK\r\nContent-Type: image/*\r\nConnection: keep-alive\r\nContent-Length: ";
			buildTextHTMLRequest("img.jpg");
			break;
	}
}
