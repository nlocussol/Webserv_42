#include <sys/fcntl>

void	make_socket_non_blocking(int fd_client)
{
	int flags;

	flags = fcntl(fd_client, F_GETFL, 0);
	if (flags == -1)
	{
		std::cerr << "flag recuperation for fd: " << fd_client << " error";
		exit (EXIT_FAILURE);
	}
	flags |= O_NONBLOCK;
	if (fcntl(fd_client, F_SETFL, flags) == -1)
	{
		std::cerr << "Set flag for fd: " << fd_client << " error";
		exit (EXIT_FAILURE);
	}
}
