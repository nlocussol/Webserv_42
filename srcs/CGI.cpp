#include "../inc/CGI.hpp"
#include "../inc/Request.hpp"
#include "../inc/webserv.hpp"
#include <fstream>
#include <unistd.h>

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

std::string CGI::handleCGI(const Request& request)
{
	if (check_cgi_args() == -1)
		return (std::string());

	// pipe(_pip);
	pipe(_pipeIn);
	pipe(_pipeOut);
	_pid = fork();
	if (_pid == -1) {
		std::cerr << "fork creation error" << std::endl;
		exit (EXIT_FAILURE);
	}
	else if (_pid == 0)
	{
		// dup2(_pip[1], 1);
		dup2(_pipeOut[1], 1);
		if (request._methodInt == GET_REQUEST) {
			// close(_pip[0]);
			// close(_pip[1]);
			char **env = vector_to_c_array(_vectorEnv);
			char *execveArgv[3] = {(char *)_binCGI.c_str(), (char *)_filePath.c_str(), NULL};
			execve(_binCGI.c_str(), execveArgv, env);
			delete [] env;
		}
		// else if (request._methodInt == POST_REQUEST) {
		// 	std::vector<std::string> envVector = string_to_vector(request._bodyContent);
		// 	char **env = vector_to_c_array(_vectorEnv);
		// 	char *execveArgv[3] = {(char *)_binCGI.c_str(), (char *)_filePath.c_str(), NULL};
		// 	dup2(_pip[0], 0);
		// 	write(0, request._buffer.c_str(), request._buffer.length());
		// 	char tmp = EOF;
		// 	write(_pip[0], &tmp, 1);
		// 	close(_pip[0]);
		// 	close(_pip[1]);
		// 	execve(_binCGI.c_str(), execveArgv, env);
		// 	delete [] env;
		// }
		else if (request._methodInt == POST_REQUEST) {
			std::vector<std::string> envVector = string_to_vector(request._bodyContent);
			char **env = vector_to_c_array(_vectorEnv);
			char *execveArgv[3] = {(char *)_binCGI.c_str(), (char *)_filePath.c_str(), NULL};

			write(_pipeIn[1], request._buffer.c_str(), request._buffer.length());

			dup2(_pipeIn[0], 0);
			dup2(_pipeOut[1], 1);

			close(_pipeIn[1]);
			close(_pipeIn[0]);
			close(_pipeOut[0]);
			close(_pipeOut[1]);
			execve(_binCGI.c_str(), execveArgv, env);
		}
		std::cerr << "execve error" << std::endl;
		close(_pipeOut[0]);
		close(_pipeOut[1]);
		close(_pipeIn[1]);
		close(_pipeIn[0]);
		exit(EXIT_FAILURE);
	}
	if (check_time() == false)
		return (string());
	return (get_output_cgi());
}

int	CGI::check_cgi_args()
{
	if (access(_filePath.c_str(), X_OK) < 0)
	{
		std::cout << "Can't use " << _filePath << " as an exectuable." << std::endl;
		_flag = PERM_DENIED;
		return (-1);
	}
	if (access(_binCGI.c_str(), X_OK) < 0)
	{
		std::cout << "Can't use " << _binCGI << " as an interpreter." << std::endl;
		_flag = PERM_DENIED;
		return (-1);
	}
	return (0);
}

bool CGI::check_time()
{
	clock_t begin = clock();
	clock_t end = clock();
	double time = 0;
	int	check = 0;
	int	wstatus;
	while (check == 0)
	{
		if ((check = waitpid(_pid, &wstatus, WNOHANG)) == -1)
		{
			std::cerr << "waitpid error" << std::endl;
			close(_pipeOut[0]);
			close(_pipeOut[1]);
			close(_pipeIn[0]);
			close(_pipeIn[1]);
			return false;
		}
		end = clock();
		time = static_cast<double>(end - begin) / CLOCKS_PER_SEC;
		if (time > MAX_CGI_WAITING)
		{
			kill (_pid, SIGKILL);
			close(_pipeOut[0]);
			close(_pipeOut[1]);
			close(_pipeIn[0]);
			close(_pipeIn[1]);
			std::cout << "cgi took more than 3sec to be executed" << std::endl;
			_flag = TIME_OUT;
			return (false);
		}
	}
	close(_pipeOut[1]);
	close(_pipeIn[0]);
	close(_pipeIn[1]);
	return (true);
}

std::string CGI::get_output_cgi()
{
	int ret;
	char	buff[BUFFER_SIZE];
	std::string	out;

	memset(buff, 0, BUFFER_SIZE);
	while ((ret = read(_pipeOut[0], buff, BUFFER_SIZE - 1) > 0))
	{
		out += buff;
		memset(buff, 0, BUFFER_SIZE);
	}
	cout << "ici\n" << out << "\n";
	if (ret == -1)
	{
		_flag = RUNTIME_ERROR;
		cout << "Read error" << endl;
		return (string());
	}
	return (out);
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
