#include "Epoll.hpp"
#include <strings.h>
#include <cstdlib>
#include <cstdio>

Epoll::Epoll(){
	_fd_epoll = -1;
	bzero(&_event, sizeof(struct epoll_event));
}

Epoll::~Epoll(){
}

Epoll::Epoll(const Epoll &copy){
	*this = copy;
}

Epoll & Epoll::operator=(const Epoll &copy){
	if (&copy != this)
	{

	}
	return (*this);
}

void	Epoll::create_epoll()
{
	_fd_epoll = epoll_create1(0);
	if (_fd_epoll == -1){
		std::cerr << "Failed to create epoll fd" << std::endl;
		exit(EXIT_FAILURE);
	}
}

void	Epoll::add_fd_to_pool(int fd)
{
	_event.events = EPOLLIN | EPOLLET;
	_event.data.fd = fd;
	if (epoll_ctl(_fd_epoll, EPOLL_CTL_ADD, fd, &_event) == -1)
	{
		perror("Add a new fd into the fd poll error\n");
		exit (EXIT_FAILURE);
	}
}

int		Epoll::get_fd_epoll()
{
	return (_fd_epoll);
}
