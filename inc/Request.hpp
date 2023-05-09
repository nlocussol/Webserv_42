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
	 void findRequestType();
 	 void findRequestSubType();
 	 bool parseRequestLine(void);
	 bool isMethodAllowed(std::string&, data&, int);
	 bool isFileProtected() const;
 	 void handleGetRequest();
 	 void handleQuery();
 	 void handlePostRequest(vector<string> &);
	 bool handleUpload(vector<string> &);
	 bool dlImage(string &, vector<string> &, int);
 	 void handleDeleteRequest();

	private:
	 Request();
	 MULTIMAP::iterator _root;
	 MULTIMAP::iterator _index;
	 int _requestType;
	 int _requestSubType;
	 int _statusCode;
	 bool _query;
	 bool _autoindex;
	 std::vector<std::string> _requestLine;
	 std::string _uri;
	 std::string _rootPath;
	 std::string _filePath;
	 std::string _queryString;
	 std::vector<std::string> _queryArg;
	 std::vector<std::string> _lines;
	 std::string _buffer;
	 std::map<std::string, std::string> _headerMap;
	 data _servers;
	 int _serverFd;
 } ;

#endif // REQUEST_HPP_
