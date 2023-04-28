#include "../inc/Request.hpp"
#include "../inc/webserv.hpp"
#include <string>

vector<string> mysplit(string & line, string delimiter);

Request::Request()
{
}

Request::~Request()
{
}

void Request::parseRequest()
{
	// std::vector<std::string> lines = mysplit(_buffer, "\n");
	// std::vector<std::string> first_line = mysplit(lines[0], " ");
	// std::cout << first_line[0] << "\n\n";
}

void Request::findRequestType(void)
{
	//Need to parse only on first line, else METHOD could be in body
	if (_buffer.find("GET") != std::string::npos)
		_requestType = GET_REQUEST;
	else if (_buffer.find("POST") != std::string::npos)
		_requestType = POST_REQUEST;
	else if (_buffer.find("DELETE") != std::string::npos)
		_requestType = DELETE_REQUEST;
	else 
		_requestType = UNSUPPORTED_REQUEST;
}

void Request::findRequestSubType(void)
{
	if (_buffer.find("Accept: text") != std::string::npos) 
		_requestSubType = TEXT;
	else if (_buffer.find("Accept: image") != std::string::npos) 
		_requestSubType = IMAGE;
	else
		_requestSubType = 1;
	//1 is equal to IMAGE, browser accepts everything idk but need to implement better
}

int Request::getRequestType(void) const 
{
	return _requestType;
}

int Request::getRequestSubType(void) const
{
	return _requestSubType;
}

void Request::setBuffer(const std::string& buffer)
{
	_buffer = buffer;
}
