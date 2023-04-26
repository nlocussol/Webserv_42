#include <iostream>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int	handle_cgi(std::string inter, std::string exec);

int main ()
{
	std::string path("/usr/bin/python3");
	std::string file("test.py");
	int			fd;
	std::ostringstream	ret;

	fd = handle_cgi(path, file);
	char buf[2600] = {0};
	if(read(fd, buf, 2600))
		std::cout << buf << std::endl;
	std::cout << "error";
	close(fd);
}

int	handle_cgi(std::string inter, std::string exec)
{
	int	pip[2];
	int	pid;
	int	wstatus;
	char	*param[3] = {(char *)inter.c_str(), (char *)exec.c_str(), NULL};

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
	if (waitpid(pid, &wstatus, 0) == -1)
	{
		perror("wait pid error");
		close(pip[0]);
		close(pip[1]);
		exit(EXIT_FAILURE);
	}
	close(pip[1]);
	return (pip[0]);
}
