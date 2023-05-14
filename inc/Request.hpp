#ifndef REQUEST_HPP_
#define REQUEST_HPP_

#include <string>
#include <vector>
#include "parsing.hpp"

class Request { 
	friend class Server;
	friend class Response;
	friend class CGI;
	friend std::ostream& operator<<(std::ostream&, const Request&);

	public:
	 Request(std::string&, data&, int);
	 ~Request();
	 void parseRequest();
	 bool basicRequestParsing();
	 bool fillMapHeader();
	 bool parseURI();
 	 bool parseRequestLine();
	 bool isMethodAllowed();
	 bool checkBasicRedirection();
 	 void handleGetRequest();
 	 void handlePostRequest();
 	 void handleChunkedTransfer();
	 bool handleUpload();
 	 void handleDeleteRequest();
	 bool checkBodySize();
	 bool checkRewrite();
	 void parseHeader();

	 int _methodInt;

	private:
	 typedef std::map<std::string, std::string>::const_iterator map_it;
	 typedef std::pair<bool, std::string> is_set;

	 Request();
	 std::string _buffer;
	 std::string _method;
	 std::string _uri;
	 std::string _httpVersion;
	 std::string _filePath;
	 std::string _queryString;
	 std::string _extension;
	 std::string _cookies;
	 std::string _serverName;
	 std::string _portNb;
	 std::string _bodyContent;
	 is_set _cookie;
	 is_set _query;
	 is_set _contentLength;
	 is_set _contentType;
	 is_set _cgi;
	 std::string _cgiBody;
	 std::string _cgiAdditionalHeader;
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
