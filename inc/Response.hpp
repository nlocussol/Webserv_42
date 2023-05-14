#ifndef RESPONSE_HPP_
#define RESPONSE_HPP_

#include <string>
#include "ErrorPage.hpp"
#include "Request.hpp"
#include "parsing.hpp"
#include <cstdlib>
#include <cstdio>

class Response { 

	public:
	 void buildResponse(Request&);
	 std::string& getCompleteResponse(void);
	 Response(block_serv, std::string&);
	 ~Response();

	private:
	 void handleRedirection(Request&);
	 void handleGetCGI(const Request&);
	 void buildCompleteResponse(int, bool);
	 void buildGetHeader(const std::string&);
	 void buildGetBody(std::string&);
	 void handleDirectoryListing(const std::string&);
	 void buildPostHeader(const Request&);
	 void buildErrorResponse(Request &);

	private:
	 std::pair<std::string, std::string> _contentType;
	 std::pair<std::string, int> _contentLength;
	 std::pair<std::string, std::string> _connection;
	 std::string _serverName;
	 std::string _cgiAdditionalHeader;
	 std::string _binaryData;
	 std::string _completeResponse;
	 std::string _location;
	 std::string _filePath;
	 block_serv _server;

	 static std::string _CRLF;
	 static std::string _HTTPVersion;
 } ;

#endif // RESPONSE_HPP_
