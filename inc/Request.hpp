#ifndef REQUEST_HPP_
#define REQUEST_HPP_

#include <string>
#include <vector>
#include "parsing.hpp"

class Request { 
	friend class Server;
	friend class Response;
	friend std::ostream& operator<<(std::ostream&, const Request&);

	public:
	 Request(std::string&, data&, int);
	 ~Request();
	 void parseRequest();
	 bool basicRequestParsing();
	 bool fillMapHeader();
	 bool parseURI();
 	 void findRequestSubType();
 	 bool parseRequestLine();
	 bool isMethodAllowed();
	 bool checkBasicRedirection();
 	 void handleGetRequest();
 	 void handleQuery();
 	 void handlePostRequest();
 	 void handleChunkedTransfer();
	 bool handleUpload();
	 bool dlImage(string &, vector<string> &, int);
 	 void handleDeleteRequest();
	 bool checkBodySize();
	 bool checkRewrite();

	 int _methodInt;
	 std::string _bodyContent;
	 std::vector<std::string> _queryArg;

	private:
	 typedef std::map<std::string, std::string>::const_iterator map_it;

	 Request();
	 std::string _buffer;
	 std::string _method;
	 std::string _uri;
	 std::string _httpVersion;
	 std::string _filePath;
	 std::string _queryString;
	 std::string _extension;
	 std::string _cgiBody;
	 bool _query;
	 bool _cgi;
	 bool _dirList;
	 int _requestSubType;
	 MULTIMAP::iterator _root;
	 MULTIMAP::iterator _index;
	 int _statusCode;
	 size_t _headerEnd;
	 bool _autoindex;
	 std::vector<std::string> _requestLine;
	 std::string _rootPath;
	 std::vector<std::string> _lines;
	 std::map<std::string, std::string> _headerMap;
	 data _servers;
	 int _serverId;
 } ;

#endif // REQUEST_HPP_
