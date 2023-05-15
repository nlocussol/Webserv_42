#include "../inc/Request.hpp"
#include "../inc/CGI.hpp"
#include "../inc/webserv.hpp"

vector<string> mysplit(string & line, string delimiter);

Request::Request()
{
}

Request::~Request()
{
}

Request::Request(std::string& buffer, data& servers, int serverFd, int clientFd, bool cgiOver, map<int, int>& pids)
{
	_statusCode = 0;
	_query.first = false;
	_dirList = false;
	_cookie.first = false;
	_cgi.first = false;
	_cgiOver = cgiOver;
	_contentType.first = false;
	_contentLength.first = false;
	_requestSubType = 0;
	_buffer = buffer;
	_servers = servers;
	_serverId = serverFd;
	_clientFd = clientFd;
	_root = _servers.v_serv[_serverId].conf_serv.find("root");
	_rootPath = _root->second;
	_index = _servers.v_serv[_serverId].conf_serv.find("index");
	_pid = pids;

	MULTIMAP::iterator autoindex = _servers.v_serv[_serverId].conf_serv.find("autoindex");
	if (autoindex == _servers.v_serv[_serverId].conf_serv.end() || autoindex->second == "off") 
		_autoIndex = false;
	else
		_autoIndex = true;
}

void Request::parseRequest()
{
	if (!basicRequestParsing())
		return ;
	if (!parseRequestLine())
		return ;
	if (!parseURI())
		return ;
	if (!isMethodAllowed())
		return ;
	if (!fillMapHeader())
		return ;
	if (!checkExpectHeader())
		return ;
	if (!checkRewrite())
		return ;
	parseHeader();
	switch (_methodInt) {
		case GET_REQUEST:
			handleGetRequest();
			break;
		case POST_REQUEST:
			handlePostRequest();
			break;
		case DELETE_REQUEST:
			handleDeleteRequest();
			break;
	}
}


bool Request::basicRequestParsing()
{
	// Find end of header's request, if not present it is not a valid HTTP request
	_headerEnd = _buffer.find("\r\n\r\n");
	if (_headerEnd == std::string::npos) {
		_statusCode = 400;
		return false;
	}
	// Split on every line return, basic HTTP request we accept need at least 2 lines
	// 1 is the request line and 1 header line
	std::string header = _buffer.substr(0, _headerEnd + 4);
	_lines = mysplit(header, "\n");
	size_t nbLines = _lines.size();
	if (nbLines < 2) {
		_statusCode = 400;
		return false;
	}
	return true;
}

// Check if request line is in METHOD URI HTTP/X.X format
bool Request::parseRequestLine()
{
	_requestLine = mysplit(_lines[0], " ");
	if (_requestLine.size() != 3) {
		_statusCode = 400;
		return false;
	}
	_method = _requestLine[0];
	if (_method == "GET")
		_methodInt = GET_REQUEST;
	else if (_method == "POST")
		_methodInt = POST_REQUEST;
	else if (_method == "DELETE")
		_methodInt = DELETE_REQUEST;
	else {
		_methodInt = UNSUPPORTED_REQUEST;
		_statusCode = 405;
		return false;
	}
	_uri = _requestLine[1];
	// Remove last character of HTTP request version (\r)
	_requestLine[2].erase(_requestLine[2].length() - 1);
	_httpVersion = _requestLine[2];
	if (_httpVersion != "HTTP/1.1" && _httpVersion != "HTTP/1.0") {
		_statusCode = 400;
		return false;
	}
	return true;
}

bool Request::fillMapHeader()
{
	// Fill map header, ignore request line because we parse it later
	// Break if end of header is found, ignore whatever else is on this line
	std::string key, value;
	for (size_t i = 1 ; _lines[i] != "\r" ; ++i) {
		// Find first ": " if not found send a bad request
		size_t colonPos = _lines[i].find(": ");
		if (colonPos == std::string::npos) {
			_statusCode = 400;
			return false;
		}
		key = _lines[i].substr(0, colonPos);
		value = _lines[i].substr(colonPos + 2, _lines[i].length());
		// Erase \r at end of line
		value.erase(value.length() - 1);
		_headerMap.insert(std::make_pair(key, value));
	}
	return true;
}

bool Request::checkExpectHeader()
{
	map_it it = _headerMap.find("Expect");
	if (it != _headerMap.end() && it->second.find("100") != std::string::npos) {
		_statusCode = 100;
		return false;
	}
	return true;
}

bool Request::parseURI()
{
	_statusCode = 200;
	_rootPath.erase(_rootPath.end() - 1);
	_filePath = _uri.substr(0, _uri.find_first_of("?"));
	if (_filePath == "/")	{
		if (_index == _servers.v_serv[_serverId].conf_serv.end()) {
			_filePath = _root->second;
			return true;
		}
		else {
			_filePath = _root->second + _index->second;
			return true;
		}
	}
	if (_uri.length() > MAX_URI_SIZE) {
		_statusCode = 414;
		return false;
	}
	// Remove first /
	_filePath.erase(0, 1);
	// Insert root path if not already in URI
	if (_filePath.compare(0, _rootPath.length(), _rootPath)) {
		_filePath.insert(0, _rootPath);
		// Add a / between root and file path cause client are dumb mfs
		if (_filePath[_rootPath.length()] != '/')
			_filePath.insert(_rootPath.length(), "/");
	}
	while (_filePath.find("%20") != std::string::npos)
		_filePath.replace(_filePath.find("%20"), 3, " ");
	size_t lastDotPos = _filePath.find_last_of(".");
	if (lastDotPos != std::string::npos)
		_extension = _filePath.substr(lastDotPos + 1);
	if (_uri.find("?") != std::string::npos) {
		_query.first = true;
		_query.second = _uri.substr(_uri.find_first_of("?") + 1);
	}
	_cgi.second = is_cgi(_servers.v_serv[_serverId], _filePath);
	if (!_cgi.second.empty())
		_cgi.first = true;
	return true;
}

bool Request::isMethodAllowed()
{
	//Check if method is allowed on this path
	MULTIMAP copy = find_location_path(_filePath, _servers.v_serv[_serverId]);
	MULTIMAP::iterator it;
	it = copy.find("methods");
	while (it != copy.end()) {
		if (it->second == _method)
			return true;
		copy.erase(it);
		it = copy.find("methods");
	}
	_statusCode = 405;
	return false;
}

bool Request::checkRewrite() {
	MULTIMAP copy = find_location_path(_filePath, _servers.v_serv[_serverId]);
	MULTIMAP::iterator it = copy.find("rewrite");
	MULTIMAP::iterator autoindex = copy.find("autoindex");
	if (it != copy.end() && autoindex != copy.end() && autoindex->second == "on") {
		_filePath = it->second;
		_statusCode = 301;
		return false;
	}
	return true;
}

void Request::parseHeader()
{
	map_it it = _headerMap.find("Cookie");
	if (it != _headerMap.end()) {
		_cookie.first = true;
		_cookie.second = it->second;
	}
	it = _headerMap.find("Content-Length");
	if (it != _headerMap.end()) {
		_contentLength.first = true;
		_contentLength.second = it->second;
	}
	it = _headerMap.find("Host");
	if (it != _headerMap.end()) {
		if (it->second.find(":") != std::string::npos) {
			_serverName = it->second.substr(0, it->second.find(":"));
			_portNb = it->second.substr(it->second.find(":") + 1);
		}
	}
	it = _headerMap.find("Content-Type");
	if (it != _headerMap.end()) {
		_contentType.first = true;
		_contentType.second = it->second;
	}
}

void Request::CGIHandler() {
	if (_cgiOver == false) {
		CGI cgi(_cgi.second, _filePath, *this);
		cgi.setClientFd(_clientFd);
		_cgiFd = cgi.handleCGI(*this);
		if (_cgiFd == -1)
		{
			if (cgi.getFlag() == TIME_OUT)
				_statusCode = 508;
			else if (cgi.getFlag() == PERM_DENIED)
				_statusCode = 403;
			else if (cgi.getFlag() == RUNTIME_ERROR)
				_statusCode = 500;
			std::cerr << "Error in cgi\n";
		}
		else
			throw (_cgiFd);
		return ;
	}
	if (_cgiOver == true) {
		if (_method == "GET") {
			std::string tmp = _buffer.substr(0, _buffer.find("\r\n\r\n"));
			_cgiAdditionalHeader = _buffer.substr(tmp.length() + 4, tmp.find("\r\n\r\n"));
			_cgiBody = _buffer.substr(_cgiAdditionalHeader.length());
			_statusCode = 200;
		}
		else if (_method == "POST") {
			std::string tmp = _buffer.substr(_buffer.find("\r\n\r\n"));
			_cgiAdditionalHeader = tmp.substr(strtol(_contentLength.second.c_str(), NULL, 10) + 4);
			_cgiBody = _cgiAdditionalHeader.substr(_cgiAdditionalHeader.find("\r\n\r\n") + 4);
			_cgiAdditionalHeader = _cgiAdditionalHeader.substr(0, _cgiAdditionalHeader.find("\r\n\r\n"));
		}
		else {
			std::cerr << "Error in CGI Method\n";
			_statusCode = 400;
		}
	}
}

void Request::handleGetRequest()
{
	if (_cgi.first)
		CGIHandler();
	if (!_filePath.empty() && is_dir_listing(_filePath, _servers.v_serv[_serverId]) == AUTOINDEX_OK) {
		_statusCode = 200;
		_dirList = true;
		return ;
	}
	// Return 404 Not Found if the file does not exist
	if (access(_filePath.c_str(), F_OK))
		_statusCode = 404;
	// Return 403 Forbidden if the file is not readable
	else if (access(_filePath.c_str(), R_OK))
		_statusCode = 403;
	// Respond 403 if URI is a directory but autoindex is off
	else if (is_dir_listing(_filePath, _servers.v_serv[_serverId]) == AUTOINDEX_OFF)
		_statusCode = 403;
	else
		_statusCode = 200;
}

void Request::handlePostRequest()
{
	if (!checkBodySize()) {
		_statusCode = 507;
		return ;
	}
	size_t bodyBegin = _buffer.find("\r\n\r\n");
	_bodyContent = _buffer.substr(bodyBegin + 4);
	map_it it;
	it = _headerMap.find("Transfer-Encoding");
	if (it != _headerMap.end() && it->second == "chunked")
		handleChunkedTransfer();
	it = _headerMap.find("Content-Type");
	if (it == _headerMap.end()) {
		_statusCode = 400;
		return ;
	}
	if (_cgi.first) {
		CGIHandler();
		return ;
	}
	if (!handleUpload())
		return ;
}

bool	Request::checkBodySize()
{
	if (_servers.v_serv[_serverId].conf_serv.find("limit_client_body_size") == _servers.v_serv[_serverId].conf_serv.end())
		return true;

	size_t	len;
	size_t	limitLen;
	char	*check;
	map_it it = _headerMap.find("Content-Length");
	limitLen = strtol(_servers.v_serv[_serverId].conf_serv.find("limit_client_body_size")->second.c_str(), &check, 10);
	if (it != _headerMap.end())
	{
		len = strtol(it->second.c_str(), &check, 10);
		if (it->second.c_str() == check || len > limitLen)
			return (false);
		return (true);
	}
	if (_bodyContent.length() > limitLen)
		return (false);
	return (true);
}

void Request::handleChunkedTransfer()
{
	int hexa_value = 0;
	int cnt;
	size_t CRLF = _buffer.find("\r\n\r\n");
	if (CRLF == std::string::npos) {
		_statusCode = 400;
		return ;
	}
	string body = _buffer.substr(CRLF + 4);
	vector<string> tab = mysplit(body, "\n");
	_bodyContent.clear();
	for (unsigned long i = 0; i < tab.size(); i++) {
		if (i % 2 == 0) {
			hexa_value = checkHexa(tab[i], "0123456789abcdef");
			if (hexa_value < 0) {
				_statusCode = 400;
				return ;
			}
		}
		else {
			cnt = 0;
			if (hexa_value == 0 && tab[i] == "\r")
				return ;
			for (; tab[i][cnt]; cnt++) {}
			if (cnt - 1 != hexa_value) {
				_statusCode = 400;
				return ;
			}
			_bodyContent += tab[i];
		}	
	}
}

bool Request::handleUpload()
{
	map_it it = _headerMap.find("Content-Type");
	if (it == _headerMap.end())
		return false;
	if (it->second.find("boundary=") == std::string::npos) {
		_statusCode = 400;
		return false;
	}
	std::string boundary = it->second.substr(it->second.find("boundary=") + 9);
	boundary.erase(0, boundary.find_last_of('-') + 1);
	size_t boundaryPos = _bodyContent.find(boundary); 
	if (boundaryPos == std::string::npos) {
		_statusCode = 400;
		return false;
	}
	size_t fileNamePos = _bodyContent.find("filename=\"");
	if (fileNamePos == std::string::npos) {
		_statusCode = 400;
		return false;
	}
	std::string fileName = _bodyContent.substr(fileNamePos + 10);
	size_t quotePos = fileName.find("\"");
	if (quotePos == std::string::npos) {
		_statusCode = 400;
		return false;
	}

	size_t doubleCRLFPos = _bodyContent.find("\r\n\r\n");
	if (doubleCRLFPos == std::string::npos) {
		_statusCode = 400;
		return false;
	}
	_bodyContent = _bodyContent.substr(doubleCRLFPos + 4);
	boundaryPos = _bodyContent.find(boundary); 
	if (boundaryPos == std::string::npos) {
		_statusCode = 400;
		return false;
	}
	MULTIMAP copy = find_location_path(_filePath, _servers.v_serv[_serverId]);
	MULTIMAP::iterator it2 = copy.find("root");
	fileName = fileName.substr(0, quotePos);
	std::ofstream of((it2->second + fileName).c_str());
	if (!of) {
		_statusCode = 500;
		return false;
	}
	std::string content = _bodyContent.substr(0, boundaryPos);
	size_t i = content.length() - 1;
	for (; content[i] == '-' ; i--) ;
	content = content.substr(0, i + 1);
	of << content;
	of.close();
	_statusCode = 303;
	_requestSubType = UPLOAD_FILE;
	return true;
}

void Request::handleDeleteRequest()
{
	if (_filePath.find("/../") != string::npos || _filePath.find("/..") != string::npos ||
			_filePath.find("../") != string::npos) {
		_statusCode = 500;
		cerr << "A file can't contain .. !" << endl;
		return ;
	}
	if (remove(_filePath.c_str()) < 0) {	
		string root = _filePath.substr(0, _filePath.find('/') + 1);
		if (root == "srcs/" || root == "inc/" || root == "conf/")
			cerr << "Can't remove file in " << root << endl;
		else
			cerr << "Can't remove " << _filePath << endl;
		_statusCode = 500;
		return ;
	}
	_statusCode = 204;
}

void	Request::addPid(int pid){ _pid.insert(make_pair(_clientFd, pid));  }

// Overload for ostream to print parsed request;
std::ostream& operator<< (std::ostream& os, const Request& request)
{
	os << "Method: " << request._method << "\n";
	os << "Uri: " << request._uri << "\n";
	os << "Version: " << request._httpVersion << "\n";
	Request::map_it it = request._headerMap.begin();
	Request::map_it ite = request._headerMap.end();
	while (it != ite) {
		os << "Header: " << it->first << ": " << it->second << "\n";
		it++;
	}
	return os;
}
