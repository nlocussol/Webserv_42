#include "../inc/Request.hpp"
#include "../inc/webserv.hpp"
#include <cstdlib>
#include <cstring>
#include <unistd.h>

/**
 *
 * parameters:
 *  - map -> std::multimap of the current server configuration
 *  - file -> the file path from the GET request sended by the client
 *
 * func:
 * 	check if the file has the cgi extension given in the parsing
 * 	return true or false in each case
 *
**/ 



int	check_cgi_args(std::string& binCGI, std::string& appPath, int *flag)
{

	if (access(appPath.c_str(), X_OK) < 0)
	{
		std::cout << "Can't use " << appPath << " as an exectuable." << std::endl;
		(*flag) = PERM_DENIED;
		return (-1);
	}
	if (access(binCGI.c_str(), X_OK) < 0)
	{
		std::cout << "Can't use " << binCGI << " as an interpreter." << std::endl;
		(*flag) = PERM_DENIED;
		return (-1);
	}
	return (0);
}

bool check_time (int pid, int pip[2], int *flag)
{
	clock_t begin = clock();
	clock_t end = clock();
	double time = 0;
	int	check = 0;
	int	wstatus;
	while (check == 0)
	{
		if ((check = waitpid(pid, &wstatus, WNOHANG)) == -1)
		{
			std::cerr << "waitpid error" << std::endl;
			close(pip[0]);
			close(pip[1]);
		}
		end = clock();
		time = static_cast<double>(end - begin) / CLOCKS_PER_SEC;
		if (time > MAX_CGI_WAITING)
		{
			kill (pid, SIGKILL);
			close(pip[1]);
			close(pip[0]);
			std::cout << "cgi took more than 3sec to be executed" << std::endl;
			(*flag) = TIME_OUT;
			return (false);
		}
	}
	close(pip[1]);
	return (true);
}

string get_output_cgi(int fd, int *flag)
{
	int ret;
	char	buff[BUFFER_SIZE];
	string	out("");

	memset(buff, 0, BUFFER_SIZE);
	while ((ret = read(fd, buff, BUFFER_SIZE) > 0))
	{
		out += buff;
		memset(buff, 0, BUFFER_SIZE);
	}
	if (ret == -1)
	{
		(*flag) = RUNTIME_ERROR;
		cout << "Read error" << endl;
		return (string());
	}
	return (out);
}

/**
 *
 * parameters:
 *  - map -> std::multimap of the current server configuration
 *  - exec -> the file path from the GET request sent by the client
 *
 * func:
 * 	
 * 	get the cgi executable path and join with the file path given as parametre
 * 	create a pipe which get the output of the program
 *  execve the program 
 *  but if the execution exceed 3seconds this will be stopped
 *
 * 	return a fd which contains the cgi output
 *
**/ 

std::string handle_cgi(std::string binCGI, std::string appPath, int *flag, Request& request)
{
	int	pip[2];
	int	pid;
	char	*param[3] = {(char *)binCGI.c_str(), (char *)appPath.c_str(), NULL};

	if (check_cgi_args(binCGI, appPath, flag) == -1)
		return (std::string());
	pipe(pip);
	pid = fork();
	if (pid == -1)
	{
		std::cerr << "fork creation error" << std::endl;
		exit (EXIT_FAILURE);
	}
	else if (pid == 0)
	{
		dup2(pip[1], 1);
		close(pip[0]);
		close(pip[1]);
		if (request._methodInt == GET_REQUEST) {
			// const char *envp[2] = {"HTTP_COOKIE=pref_lang=fr", NULL};
			char** env = vector_to_c_array(request._queryArg);
			execve(binCGI.c_str(), param, const_cast<char **>(env));
			delete [] env;
		}
		else if (request._methodInt == POST_REQUEST && request._bodyContent.size() > 0) {
			std::vector<std::string> envVector = string_to_vector(request._bodyContent);
			char **env = vector_to_c_array(envVector);
			execve(binCGI.c_str(), param, env);
			delete [] env;
		}
		std::cerr << "execve error" << std::endl;
		close(pip[0]);
		close(pip[1]);
		exit(EXIT_FAILURE);
	}
	if (check_time(pid, pip, flag) == false)
		return (string());
	return (get_output_cgi(pip[0], flag));
}
