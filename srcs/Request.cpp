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

Request::Request(std::string& buffer)
{
	_statusCode = 0;
	_query = false;
	_buffer = buffer;
}

void Request::parseRequest(data& servers, int serv)
{
	if (_buffer.length() == 0) {
		_statusCode = 200;
		return ;
	}
	std::vector<std::string> lines = mysplit(_buffer, "\n");
	std::vector<std::string> first_line = mysplit(lines[0], " ");
	if (first_line[1].find("?"))
		_query = true;
	if (!isAllowedMethod(first_line[0], servers, serv)) {
		_statusCode = 405;
		return ;
	}
	 _statusCode = 200;

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

bool Request::isAllowedMethod(std::string& method, data& servers, int server)
{
	MULTIMAP copy = servers.serv[server].conf;
	MULTIMAP::iterator it;

	it = copy.find("methods");
	while (it != copy.end()) {
		if (it->second == method)
			return true;
		copy.erase(it);
		it = copy.find("methods");
	}
	return false;
}

int Request::getRequestType(void) const 
{
	return _requestType;
}

int Request::getRequestSubType(void) const
{
	return _requestSubType;
}
