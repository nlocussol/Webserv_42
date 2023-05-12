#include "../inc/Request.hpp"
#include "../inc/webserv.hpp"
#include <cstdlib>
#include <string>
#include <cstring>
#include <sstream>

vector<string> mysplit(string & line, string delimiter);

Request::Request()
{
}

Request::~Request()
{
}

Request::Request(std::string& buffer, data& servers, int serverFd)
{
	_statusCode = 0;
	_query = false;
	_dirList = false;
	_cgi = false;
	_buffer = buffer;
	_servers = servers;
	_serverId = serverFd;
	_root = _servers.v_serv[_serverId].conf_serv.find("root");
	_rootPath = _root->second;
	_index = _servers.v_serv[_serverId].conf_serv.find("index");

	MULTIMAP::iterator autoindex = _servers.v_serv[_serverId].conf_serv.find("autoindex");
	if (autoindex == _servers.v_serv[_serverId].conf_serv.end() || autoindex->second == "off") 
		_autoindex = false;
	else
		_autoindex = true;
}

void Request::parseRequest()
{
	if (!basicRequestParsing())
		return ;
	if (!parseRequestLine())
		return ;
	if (!isMethodAllowed())
		return ;
	if (!parseURI())
		return ;
	if (!fillMapHeader())
		return ;
	findRequestSubType();
	// if (!checkBasicRedirection())
		// return ;
	if (!checkRewrite())
		return ;
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

bool Request::checkRewrite() {
	MULTIMAP copy = find_location_path(_filePath, _servers.v_serv[_serverId]);
	MULTIMAP::iterator it = copy.find("rewrite");
	MULTIMAP::iterator autoindex = copy.find("autoindex");
	if (it != copy.end() && autoindex != copy.end() && autoindex->second == "on") {
		cout << "HERRREEEEEE" << endl;
		_filePath = it->second;
		_statusCode = 301;
		return false;
	}
	return true;
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

//Check if request line is in METHOD URI HTTP/X.X format
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
	// We do not support uri > 2000 characters
	if (_uri.length() > 2000) {
		_statusCode = 400;
		return false;
	}
	// Remove first /
	_filePath.erase(0, 1);
	//Insert root path if not already in URI
	if (_filePath.compare(0, _rootPath.length(), _rootPath)) {
		_filePath.insert(0, _rootPath);
		//Add a / between root and file path cause client are dumb mfs
		if (_filePath[_rootPath.length()] != '/')
			_filePath.insert(_rootPath.length(), "/");
	}
	size_t lastDotPos = _filePath.find_last_of(".");
	if (lastDotPos != std::string::npos)
		_extension = _filePath.substr(lastDotPos + 1);
	// else _dirList == true ??
	if (_uri.find("?") != std::string::npos) {
		_query = true;
		_queryString = _uri.substr(_uri.find_first_of("?") + 1);
	}

	if (_filePath.find(".py") != std::string::npos) {
		_cgi = true;
	}
	if (is_cgi(_servers.v_serv[_serverId], _filePath)) {
		_cgi = true;
	}
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

void Request::findRequestSubType()
{
	if (_query == true) {
		_requestSubType = QUERY;
		return ;
	}
	map_it it;
	it = _headerMap.find("Accept");
	if (it->second.find("image") != std::string::npos) 
		_requestSubType = IMAGE;
	else if (it->second.find("text") != std::string::npos) 
		_requestSubType = TEXT;
	else
		_requestSubType = DEFAULT;
}

// Check if the client ask for a dir with autoindex on without last /
// if so redirect to same path with /
// also check the opposite, if client ask for a file with / at the end
// if so redirect to same path without /
bool Request::checkBasicRedirection()
{
	if (is_dir(_filePath) && _filePath[_filePath.size() - 1] != '/') {
		MULTIMAP copy = find_location_path(_filePath, _servers.v_serv[_serverId]);
		MULTIMAP::iterator it = copy.find("autoindex");
		if (it != copy.end() && it->second == "on") {
			_filePath.erase(0, _rootPath.size());
			_filePath += "/";
			_statusCode = 301;
		}
		return false;
	}
	if (!is_dir(_filePath) && _filePath[_filePath.size() - 1] == '/') {
		_filePath.erase(0, _rootPath.size());
		_filePath.erase(_filePath.size() - 1);
		_statusCode = 301;
		return false;
	}
	return true;
}

void Request::handleGetRequest()
{
	if (_query) {
		handleQuery();
	}
	if (_cgi) {
		int flag = 0;
		handle_cgi(_servers.v_serv[_serverId], _filePath, &flag, *this);
		if (flag == TIME_OUT)
			_statusCode = 508;
		else if (flag == PERM_DENIED)
			_statusCode = 403;
		else if (flag == RUNTIME_ERROR)
			_statusCode = 500;
		else
			_statusCode = 200;
		return ;
	}
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

void Request::handleQuery()
{
	//need to test if it works + what to do with it + need to translate + to space " " and special characters zzz
	//It is needed in some CGI where the _queryArg needs to be translated into env for execv
	_queryArg = string_to_vector(_queryString);
	// Probably can delete this below
	// while (_queryString.length() != 0) {
	// 	if (_queryString.find("&") != std::string::npos)
	// 		ampersandPos = _queryString.find_first_of("&");
	// 	else
	// 		ampersandPos = _queryString.length();
	// 	arg = _queryString.substr(0, ampersandPos);
	// 	_queryArg.push_back(arg);
	// 	_queryString.erase(0, ampersandPos + 1);
		// std::cout << _queryString.length() << "\n\n";
	// }
}

void Request::handlePostRequest()
{
	if (!checkBodySize()) {
		_statusCode = 507;
		return ;
	}
	map_it it;
	it = _headerMap.find("Transfer-Encoding");
	if (it != _headerMap.end() && it->second == "chunked") {
		handleChunkedTransfer();
		return ;
	}

	if (handleUpload())
		return ;
	// Check if the file already exists, if it does try to delete it
	std::ifstream infile;
	// Also need to test if directory is writable, but j'ai la flemme là
	infile.open(_filePath.c_str());
	if (infile) {
		if (access(_filePath.c_str(), F_OK | R_OK)) {
			_statusCode = 403;
			return ;
		}
		else {
	// Need to add proper error code return, but file should be deletable since we access it upward|
			if (std::remove(_filePath.c_str())) {
				_statusCode = 0;
				return ;
			}
		}
	}
	// Create file with the same name and fill it with body of POST request
	size_t bodyBegin = _buffer.find("\r\n\r\n");
	// need to check if find worked
	_bodyContent = _buffer.substr(bodyBegin + 4);
	if (_cgi) {
		int flag;
		handle_cgi(_servers.v_serv[_serverId], _filePath, &flag, *this);
	}
	std::ofstream outfile(_filePath.c_str());
	outfile << _bodyContent;
	outfile.close();
	_statusCode = 200;
}

bool	Request::checkBodySize()
{
	if (_servers.v_serv[_serverId].conf_serv.find("limit_client_body_size") == _servers.v_serv[_serverId].conf_serv.end())
		return true;

	map_it it;
	size_t	len;
	size_t	limitLen;
	char	*check;

	it = _headerMap.find("Content-Length");
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
	string body = _buffer.substr(_buffer.find("\r\n\r\n") + 4);
	vector<string> tab = mysplit(body, "\n");
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
		}	
	}
}

bool Request::dlImage(std::string & id, std::vector<std::string> & lines, int i)
{
	std::ofstream file;
	for (; lines[i].find(id) == std::string::npos; i++) { }
	i++;
	size_t j = lines[i].find("filename=\"") + 10;
	std::string file_name;
	for (; lines[i][j] && lines[i][j] != '"'; j++)
		file_name += lines[i][j];
	file.open(file_name.c_str());
	if (!file) {
		std::cout << "Can't upload file because the file can't be created" << std::endl;
		return false;
	}
	i += 3;
	std::string upload = _buffer.substr(_buffer.find("Content-Disposition:"));
	std::string str = upload.substr(upload.find("Content-Type:"));
	upload = str.substr(str.find("\n") + 3);
	file << upload;
	file.close();
	return true;
}

bool Request::handleUpload()
{
	// map_it it = _headerMap.find("Content-Type");
	// if (it == _headerMap.end())
	// 	return false;
	// std::string
	for (size_t i = 0; i < _lines.size(); i++) {
		if (_lines[i].find("boundary=") != string::npos) {
			string str = _lines[i].substr(_lines[i].find("boundary=") + 9);
			string id = str.substr(str.find_last_of('-') + 1);
			if (!dlImage(id, _lines, i + 1))
				return false;
			return (true);
		}
	}
	return (false);
}

void Request::handleDeleteRequest()
{
	if (remove(_filePath.c_str()) < 0) {	
		string root = _filePath.substr(0, _filePath.find('/') + 1);
		if (root == "srcs/" || root == "inc/" || root == "conf/")
			cout << "Can't remove file in " << root << endl;
		else
			cout << "Can't remove " << _filePath << endl;
	}
	_statusCode = 200;
}
