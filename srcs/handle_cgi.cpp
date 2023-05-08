#include "../inc/webserv.hpp"
#include <cstdlib>
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

bool	is_cgi(block_serv server, std::string file)
{
	std::string	extend;	
	std::size_t		check;
	string path;
	MULTIMAP copy;

	if (file.find('/') != string::npos)
		path = file.erase(file.find_last_of('/'), file.size());
	else
		path = "/";
	copy = find_location_path(path, server);
	if (copy.find("cgi") == copy.end())
		return (false);
	copy.erase(copy.find("cgi")); 
	extend = copy.find("cgi")->second; 
	check = file.rfind(extend);
	if (check != std::string::npos && check + extend.length() == file.size() && extend.length() != file.length())
	{
		return (true);
	}
	return (false);
}

int	check_cgi_args(std::string inter, std::string exec, int *flag)
{

	if (access(exec.c_str(), X_OK) < 0)
	{
		std::cout << "Can't use " << exec << " as an exectuable." << std::endl;
		(*flag) = PERM_DENIED;
		return (-1);
	}
	if (access(inter.c_str(), X_OK) < 0)
	{
		std::cout << "Can't use " << inter << " as an interpreter." << std::endl;
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
	string	out;

	while ((ret = read(fd, buff, BUFFER_SIZE) > 0))
		out += buff;
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

string handle_cgi(block_serv server, std::string exec, int *flag)
{
	int	pip[2];
	int	pid;
	MULTIMAP map;
	map = find_location_path(exec, server);
	std::string inter = map.find("cgi")->second;
	string str = map.find("root")->second + "/" + exec;
	char	*param[3] = {(char *)inter.c_str(), (char *)str.c_str(), NULL};

	if (check_cgi_args(inter, str, flag) == -1)
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
		execve(inter.c_str(), param, NULL);
		std::cerr << "execve error" << std::endl;
		close(pip[0]);
		close(pip[1]);
		exit(EXIT_FAILURE);
	}
	if (check_time(pid, pip, flag) == false)
		return (string());
	return (get_output_cgi(pip[0], flag));
}
