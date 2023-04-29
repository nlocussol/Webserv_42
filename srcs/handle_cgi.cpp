#include "../inc/webserv.hpp"

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

bool	is_cgi(MULTIMAP map, std::string file)
{
	MULTIMAP copy = map;	
	std::string	extend;	
	std::size_t		check;

	copy.erase(copy.find("cgi")); 
	extend = copy.find("cgi")->second; 
	check = file.rfind(extend);
	if (check != std::string::npos && check + extend.length() == file.size() && extend.length() != file.length())
	{
		return (true);
	}
	return (false);
}

int	check_cgi_args(std::string inter, std::string exec)
{
	int			fd;

	fd = open(exec.c_str(), O_RDWR);
	if (fd == -1)
	{
		std::cout << "Can't use " << exec << " as an exectuable." << std::endl;
		return (fd);
	}
	close (fd);
	fd = access(inter.c_str(), X_OK);
	if (fd == -1)
	{
		std::cout << "Can't use " << inter << " as an interpreter." << std::endl;
		return (fd);
	}
	close (fd);
	return (0);
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

int	handle_cgi(MULTIMAP map, std::string exec)
{
	int	pip[2];
	int	pid;
	int	wstatus;
	std::string inter = map.find("cgi")->second;
	char	*param[3] = {(char *)inter.c_str(), (char *)exec.c_str(), NULL};

	if (check_cgi_args(inter, exec) == -1)
		return (-1);
	pipe(pip);
	pid = fork();
	if (pid == -1)
	{
		perror("fork error");
		exit(EXIT_FAILURE);
	}
	else if (pid == 0)
	{
		dup2(pip[1], 1);
		close(pip[0]);
		close(pip[1]);
		execve(inter.c_str(), param, NULL);
		perror("execve error");
		close(pip[0]);
		close(pip[1]);
		exit(EXIT_FAILURE);
	}
	clock_t begin = clock();
	clock_t end = clock();
	double time = 0;
	int	check = 0;
	while (check == 0)
	{
		if ((check = waitpid(pid, &wstatus, WNOHANG)) == -1)
		{
			perror("wait pid error");
			close(pip[0]);
			close(pip[1]);
		}
		end = clock();
		time = static_cast<double>(end - begin) / CLOCKS_PER_SEC;
		if (time > 3)
		{
			close(pip[1]);
			close(pip[0]);
			return (-1);
		}
	}
	close(pip[1]);
	return (pip[0]);
}
