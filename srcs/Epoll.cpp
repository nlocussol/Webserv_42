#include "../inc/Epoll.hpp"

Epoll::Epoll(){
	_fd_epoll = -1;
	bzero(&_event, sizeof(struct epoll_event));
}

Epoll::~Epoll(){
	if (_fd_epoll > 0)
		close (_fd_epoll);
}

Epoll::Epoll(const Epoll &copy){
	*this = copy;
}

Epoll & Epoll::operator=(const Epoll &copy){
	if (&copy != this)
	{
		_fd_epoll = copy._fd_epoll;
		_event = copy._event;
	}
	return (*this);
}

void	Epoll::create_epoll()
{
	_fd_epoll = epoll_create1(0);
	if (_fd_epoll == -1){
		std::cerr << "Failed to create epoll fd" << std::endl;
		std::exit (1);
	}
}

void	Epoll::add_fd_to_pool(int fd)
{
	_event.events = EPOLLIN;
	_event.data.fd = fd;
	if (epoll_ctl(_fd_epoll, EPOLL_CTL_ADD, fd, &_event) == -1)
	{
		std::cerr << "Add a new fd into the fd poll error" << std::endl;
	}
}

void	Epoll::del_fd_from_pool(int fd)
{
	if (epoll_ctl(_fd_epoll, EPOLL_CTL_DEL, fd, &_event) == -1)
	{
		std::cerr << "Remove fd of epoll error" << std::endl;
	}
}

int		Epoll::get_fd_epoll()
{
	return (_fd_epoll);
}
