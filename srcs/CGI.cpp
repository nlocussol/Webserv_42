#include "../inc/CGI.hpp"

CGI::CGI(std::string& binCGI, std::string& filePath, Request request)
{
	_binCGI = binCGI;
	_filePath = filePath;
	_flag = 0;
	_vectorEnv.push_back("REQUEST_METHOD=" + request._method);
	_vectorEnv.push_back("SERVER_SOFTWARE=MbaboinServ/1.25");
	_vectorEnv.push_back("GATEWAY_INTERFACE=CGI/1.1");
	_vectorEnv.push_back("SERVER_PROTOCOL=HTTP/1.1");
	_vectorEnv.push_back("SERVER_NAME=" + request._serverName);
	_vectorEnv.push_back("SERVER_PORT=" + request._portNb);
	_vectorEnv.push_back("PATH_INFO=" + _filePath);
	if (request._contentType.first)
		_vectorEnv.push_back("CONTENT_TYPE="  + request._contentType.second);
	if (request._contentLength.first)
		_vectorEnv.push_back("CONTENT_LENGTH=" + request._contentLength.second);
	if (request._query.first)
		_vectorEnv.push_back("QUERY_STRING=" + request._query.second);
	if (request._cookie.first)
		_vectorEnv.push_back("HTTP_COOKIE=" + request._cookie.second);
	_postBody = request._bodyContent;
}

int CGI::handleCGI(Request& request)
{
	if (check_cgi_args() == -1)
		return (-1);

	pipe(_pipeIn);
	pipe(_pipeOut);
	_pid = fork();
	if (_pid == -1) {
		std::cerr << "Error: fork creation error" << std::endl;
		exit (EXIT_FAILURE);
	}
	else if (_pid == 0)
	{
		char **env = vector_to_c_array(_vectorEnv);
		char *execveArgv[3] = {(char *)_binCGI.c_str(), (char *)_filePath.c_str(), NULL};
		if (dup2(_pipeIn[0], 0) < 0 || dup2(_pipeIn[1], 1) < 0) {
			std::cerr << "Error: dup2";
			return -1;
		}
		cout << request._bodyContent;
		if (dup2(_pipeOut[1], 1) < 0) {
			std::cerr << "Error: dup2";
			return -1;
		}
		close(_pipeIn[1]);
		close(_pipeIn[0]);
		close(_pipeOut[0]);
		close(_pipeOut[1]);
		execve(_binCGI.c_str(), execveArgv, env);
		std::cerr << "Error: execve error" << std::endl;
		delete [] env;
		exit(EXIT_FAILURE);
	}
	close(_pipeOut[1]);
	close(_pipeIn[1]);
	close(_pipeIn[0]);
	request.addPid(_pid);
	return (_pipeOut[0]);
}

int	CGI::check_cgi_args()
{
	if (access(_filePath.c_str(), X_OK) < 0)
	{
		std::cerr << "Can't use " << _filePath << " as an exectuable." << std::endl;
		_flag = PERM_DENIED;
		return (-1);
	}
	if (access(_binCGI.c_str(), X_OK) < 0)
	{
		std::cerr << "Can't use " << _binCGI << " as an interpreter." << std::endl;
		_flag = PERM_DENIED;
		return (-1);
	}
	return (0);
}

string is_cgi(block_serv server, const string& filePath)
{
	string path;
	string copyFilePath = filePath;
	string	extend;	
	string	interpreter("");	
	size_t	check;
	MULTIMAP copy;

	if (filePath.find('/') != string::npos)
		path = copyFilePath.erase(filePath.find_last_of('/'), filePath.size());
	else
		path = "/";
	copy = find_location_path(path, server);
	if (copy.find("cgi") == copy.end())
		return (string(""));
	do{
		interpreter = copy.find("cgi")->second;
		copy.erase(copy.find("cgi")); 
		extend = copy.find("cgi")->second; 
		check = filePath.rfind(extend);
		if (check != std::string::npos && check + extend.length() == filePath.size() && extend.length() != filePath.length())
		{
			return (string(interpreter));
		}
		copy.erase(copy.find("cgi")); 
	} while (copy.find("cgi") != copy.end());
	return (string(""));
}

int CGI::getFlag() const {return _flag;}

void CGI::setClientFd(int clientFd) {_clientFd = clientFd;}

CGI::~CGI() {}
