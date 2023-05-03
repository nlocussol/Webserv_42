#include "../inc/Request.hpp"
#include "../inc/webserv.hpp"
#include <string>

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
	_buffer = buffer;
	_servers = servers;
	_serverFd = serverFd;
}

void Request::parseRequest(data& servers, int serv)
{
	// Client requests an empty string ??? Idk what to respond back
	if (_buffer.length() == 0) {
		_statusCode = 200;
		return ;
	}
	std::vector<std::string> lines = mysplit(_buffer, "\n");
	if (lines.size() == 0) {
		_statusCode = 400;
		return ;
	}
	std::vector<std::string> first_line = mysplit(lines[0], " ");
	if (first_line.size() != 3 || first_line[2].find("HTTP") == std::string::npos){
		_statusCode = 400;
		return ;
	}
	parseURI(first_line[1]);
	if (!isMethodAllowed(first_line[0], servers, serv)) {
		_statusCode = 405;
		return ;
	}
	findRequestType();
	if (_requestType != UNSUPPORTED_REQUEST) {
		findRequestSubType();
		switch (_requestType) {
			case GET_REQUEST:
				handleGetRequest();
				break;
			case POST_REQUEST:
				handlePostRequest(lines);
				break;
			case DELETE_REQUEST:
				handleDeleteRequest();
				break;
		}
	}
}

void Request::parseURI(const std::string& uri)
{
	MULTIMAP::iterator root = _servers.v_serv[_serverFd].conf_serv.find("root");
	MULTIMAP::iterator index = _servers.v_serv[_serverFd].conf_serv.find("index");
	_rootPath = root->second;
	_rootPath.erase(_rootPath.end() - 1);
	_filePath = uri.substr(0, uri.find_first_of("?"));
	if (_filePath == "/" && index->second != "")	{
		_filePath = root->second + index->second;
		return ;
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

	//If autoindex is turned off or not specified, return a 403 Forbidden error
	//Need to fix it ffff
	// if (!is_dir_listing(_filePath, _servers.v_serv[_serverFd])) {
	// 	_statusCode = 403;
	// 	_requestType = ERROR_RESPONSE;
	// 	std::cout <<"here\n";
	// 	return;
	// }
	if (uri.find("?") != std::string::npos) {
		_query = true;
		_queryString = uri.substr(uri.find_first_of("?") + 1);
	}
}

void Request::findRequestType(void)
{
	if (_statusCode == 0) {
		//Need to parse only on first line, else METHOD could be in body
		if (_buffer.find("GET") != std::string::npos)
			_requestType = GET_REQUEST;
		else if (_buffer.find("POST") != std::string::npos)
			_requestType = POST_REQUEST;
		else if (_buffer.find("DELETE") != std::string::npos)
			_requestType = DELETE_REQUEST;
		else 
			_requestType = UNSUPPORTED_REQUEST;
	}
}

void Request::findRequestSubType(void)
{
	if (_query == true)
		_requestSubType = QUERY;
	else if (_buffer.find("Accept: text") != std::string::npos) 
		_requestSubType = TEXT;
	else if (_buffer.find("Accept: image") != std::string::npos) 
		_requestSubType = IMAGE;
	else
		_requestSubType = 1;
	//1 is equal to IMAGE, browser accepts everything idk but need to implement better
}

void Request::handleGetRequest()
{
	if (_requestSubType == QUERY)
		handleQuery();
	/*
	 * regrade si c'est un cgi et le lance au besoin.
	 * décommenter ces fonctions quand parsing sur cgi sera fait
	 * pour l'instant, renvoie un fd mais peu renvoyer une string au besoin
	*/
	if (!_filePath.empty() && is_dir_listing(_filePath, _servers.v_serv[_serverFd]) == true) {
		_statusCode = 200;
		_requestSubType = DIR_LISTING;
		return ;
	}
	if (!_filePath.empty() && is_cgi(_servers.v_serv[_serverFd], _filePath) == true)
	{
		//need to add a typedef for CGI_HANDLER
		//+ need to pass _queryArg into char* to send to CGI, probably do this with a getter and then in server object
		handle_cgi(_servers.v_serv[_serverFd], _filePath);
	}
	// Return 404 Not Found if the file does not exist
	if (access(_filePath.c_str(), F_OK)) {
		_statusCode = 404;
		return;
	}
	// Return 403 Forbidden if the file is not readable
	if (access(_filePath.c_str(), R_OK)) {
		_statusCode = 403;
		return ;
	}
	_statusCode = 200;
}

void Request::handleQuery()
{
	std::string arg;
	size_t ampersandPos;
	//need to test if it works + what to do with it + need to translate + to space " " and special characters zzz
	//It is needed in some CGI where the _queryArg needs to be translated into env for execve 
	while (_queryString.length() != 0) {
		if (_queryString.find("&") != std::string::npos)
			ampersandPos = _queryString.find_first_of("&");
		else
			ampersandPos = _queryString.length();
		arg = _queryString.substr(0, ampersandPos);
		_queryArg.push_back(arg);
		_queryString.erase(0, ampersandPos + 1);
		std::cout << _queryString.length() << "\n\n";
	}
}

bool Request::dlImage(string & id, vector<string> & lines, int i) {
	ofstream file;
	for (; lines[i].find(id) == string::npos; i++) { }
	i++;
	size_t j = lines[i].find("filename=\"") + 10;
	string file_name;
	for (; lines[i][j] && lines[i][j] != '"'; j++)
		file_name += lines[i][j];
	file.open(file_name.c_str());
	if (!file) {
		cout << "Can't upload file because the file can't be create" << endl;
		return false;
	}
	i += 3;
	string upload = _buffer.substr(_buffer.find("Content-Disposition:"));
	string str = upload.substr(upload.find("Content-Type:"));
	upload = str.substr(str.find("\n") + 3);
	file << upload;
	file.close();
	return true;
}

bool Request::handleUpload(vector<string> & lines) {
	for (unsigned long i = 0; i < lines.size(); i++) {
		if (lines[i].find("boundary=") != string::npos) {
			string str = lines[i].substr(lines[i].find("boundary=") + 9);
			string id = str.substr(str.find_last_of('-') + 1);
			if (!dlImage(id, lines, i + 1))
				return false;
			return (true);
		}
	}
	return (false);
}

void Request::handlePostRequest(vector<string> & lines)
{
	if (handleUpload(lines))
		return ;
	// Return 403 Forbidden is the file is core of our project
	if (isFileProtected()) {
		_statusCode = 403;
		return ;
	}
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
	// Need to add proper error code return, but file should be deletable since we access it upward
			if (std::remove(_filePath.c_str())) {
				_statusCode = 0;
				return ;
			}
		}
	}
	// Create file with the same name and fill it with body of POST request
	size_t bodyBegin = _buffer.find("\r\n\r\n");
	std::string bodyContent = _buffer.substr(bodyBegin + 4);
	std::ofstream outfile(_filePath.c_str());
	outfile << bodyContent;
	outfile.close();
	_statusCode = 200;
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

bool Request::isMethodAllowed(std::string& method, data& servers, int server)
{
	MULTIMAP copy = servers.v_serv[server].conf_serv;
	MULTIMAP::iterator it;

	it = copy.find("methods");
	while (it != copy.end()) {
		if (it->second == method)
			return true;
		copy.erase(it);
		it = copy.find("methods");
	}
	return false;
}

bool Request::isFileProtected() const
{
	if (_filePath.find("utils/BIDEN_BLAST.mp4") != std::string::npos)
		return true;
	if (_filePath.find("utils/index.html") != std::string::npos)
		return true;
	if (_filePath.find("utils/favicon.ico") != std::string::npos)
		return true;
	if (_filePath.find("utils/img.jpg") != std::string::npos)
		return true;
	return false;
}
