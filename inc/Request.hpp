#ifndef REQUEST_HPP_
#define REQUEST_HPP_

#include <string>
#include "parsing.hpp"

class Request { 
	friend class Server;
	friend class Response;

	public:
	 Request();
	 Request(std::string&, data&, int);
	 ~Request();
	 void parseRequest(data&, int);
	 void findRequestType();
 	 void findRequestSubType();
	 bool isAllowedMethod(std::string&, data&, int);
	 bool isFileProtected() const;
 	 void handleGetRequest();
 	 void handlePostRequest();
 	 void handleDeleteRequest();

	private:
	 int _requestType;
	 int _requestSubType;
	 int _statusCode;
	 bool _query;
	 std::string _filePath;
	 std::string _queryString;
	 std::string _buffer;
	 data _servers;
	 int _serverFd;
 } ;

#endif // REQUEST_HPP_
