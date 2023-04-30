#ifndef REQUEST_HPP_
#define REQUEST_HPP_

#include <string>

struct data;

class Request { 
	friend class Server;
	friend class Response;

	public:
	 Request();
	 Request(std::string&);
	 ~Request();
	 void parseRequest(data&, int);
	 void findRequestType();
 	 void findRequestSubType();
	 int getRequestType(void) const; 
	 int getRequestSubType(void) const; 
	 bool isAllowedMethod(std::string&, data&, int);

	private:
	 int _requestType;
	 int _requestSubType;
	 int _statusCode;
	 bool _query;
	 std::string _filePath;
	 std::string _queryString;
	 std::string _buffer;
 } ;

#endif // REQUEST_HPP_
