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
	 Response(block_serv);
	 ~Response();

	private:
	 void handleRedirection(Request&);
	 void buildCompleteResponse(int);
	 void buildGetHeader(int);
	 void buildGetBody(std::string&, block_serv& );
	 bool check_dir(string &, block_serv &);
	 void handleDirectoryListing(const std::string&);
	 void buildPostHeader(int);
	 void buildErrorResponse(Request &);

	private:
	 std::pair<std::string, std::string> _contentType;
	 std::pair<std::string, int> _contentLength;
	 std::pair<std::string, std::string> _connection;
	 std::string _binaryData;
	 std::string _completeResponse;
	 block_serv _server;

	 static std::string _CRLF;
	 static std::string _HTTPVersion;
	 
 } ;

#endif // RESPONSE_HPP_
