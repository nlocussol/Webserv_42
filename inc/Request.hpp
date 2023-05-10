#ifndef REQUEST_HPP_
#define REQUEST_HPP_

#include <string>
#include <vector>
#include "parsing.hpp"

class Request { 
	friend class Server;
	friend class Response;

	public:
	 Request(std::string&, data&, int);
	 ~Request();
	 void parseRequest(data&, int);
	 bool fillMapHeader();
	 void parseURI();
	 bool findRequestType();
 	 void findRequestSubType();
 	 bool parseRequestLine(void);
	 bool isMethodAllowed();
 	 void handleGetRequest();
 	 void handleQuery();
 	 void handlePostRequest();
 	 void handleChunkedTransfer();
	 bool handleUpload();
	 bool dlImage(string &, vector<string> &, int);
 	 void handleDeleteRequest();
	 int checkHexa(string &, string hexa);
     bool checkBodyLength(string bodyContent);

	 int _requestType;
	 std::string _bodyContent;
	 std::vector<std::string> _queryArg;

	private:
	 Request();
	 MULTIMAP::iterator _root;
	 MULTIMAP::iterator _index;
	 int _requestSubType;
	 int _statusCode;
	 size_t _headerEnd;
	 bool _query;
	 bool _cgi;
	 bool _autoindex;
	 std::vector<std::string> _requestLine;
	 std::string _uri;
	 std::string _method;
	 std::string _rootPath;
	 std::string _filePath;
	 std::string _queryString;
	 std::vector<std::string> _lines;
	 std::string _buffer;
	 std::map<std::string, std::string> _headerMap;
	 data _servers;
	 int _serverId;
 } ;

#endif // REQUEST_HPP_
