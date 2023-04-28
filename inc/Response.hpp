#ifndef RESPONSE_HPP_
#define RESPONSE_HPP_

#include <string>
#include "Request.hpp"

class Response { 
	public:
	 void buildResponse(const Request&, const std::string&);
	 void buildCompleteResponse();
	 std::string& getCompleteResponse(void);
	 void setStatusCode(int);
	 int getStatusCode(void);
	 Response();
	 ~Response();

	private:
	 Response(const Response&);
	 Response& operator=(const Response&);
	 void buildGetHeader(int);
	 void buildGetBody(const Request&, const std::string&);
	 void buildErrorResponse();
	 std::string itostr(int);

	private:
	 int _statusCode;
	 std::pair<std::string, std::string> _contentType;
	 std::pair<std::string, int> _contentLength;
	 std::pair<std::string, std::string> _connection;
	 std::string _binaryData;
	 std::string _completeResponse;
	 static std::string _CRLF;
	 static std::string _404Page;
 } ;

#endif // RESPONSE_HPP_
