#include "../inc/Request.hpp"
#include "../inc/webserv.hpp"
#include <string>

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

void Request::findRequestType(const std::string& buffer)
{
	//Need to parse only on first line, else METHOD could be in body
	if (buffer.find("GET") != std::string::npos)
		_requestType = GET_REQUEST;
	else if (buffer.find("POST") != std::string::npos)
		_requestType = POST_REQUEST;
	else if (buffer.find("DELETE") != std::string::npos)
		_requestType = DELETE_REQUEST;
	else 
		_requestType = UNSUPPORTED_REQUEST;
}

void Request::findRequestSubType(const std::string& buffer)
{
	if (buffer.find("Accept: text") != std::string::npos) 
		_requestSubType = TEXT;
	else if (buffer.find("Accept: image") != std::string::npos) 
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
