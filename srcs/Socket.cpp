#include "../inc/Socket.hpp"
#include <exception>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdexcept>
#include <strings.h>
#include <cstdio>

 Socket::Socket(int port){
	 _port = port;
	 _fd_server = -1;
 }

 Socket::Socket(){
	 _port = 0;
	 _fd_server = -1;
 }

 Socket::~Socket(){
	 if (_fd_server > 0)
		 close (_fd_server);
 }

 Socket::Socket(const Socket &copy){
 	*this = copy;
 }

 Socket & Socket::operator=(const Socket &copy){
 	if (&copy != this)
 	{
		_fd_server = copy._fd_server;
		_port = copy._port;
		_server_addr = copy._server_addr;
 	}
 	return (*this);
 } 

int	Socket::get_fdServer()
{
	return(_fd_server);
}

int	Socket::get_port()
{
	return(_port);
}

void	Socket::allow_socket_server(int port)
{
	int	option = 1;

	_port = port;
	_fd_server = socket(AF_INET, SOCK_STREAM, 0);
	if (_fd_server < 0)
		throw std::runtime_error("creating socket error");
	bzero(&_server_addr, sizeof(struct sockaddr_in));
	_server_addr.sin_family = AF_INET;
	_server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	_server_addr.sin_port = htons(_port);
	if (setsockopt(_fd_server, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(int)) < 0)
		throw std::runtime_error("setsockopt error");
	if (bind(_fd_server, (struct sockaddr *)&_server_addr, sizeof(_server_addr)) < 0)
		throw std::runtime_error("bind socket to the right port error");
	if (listen(_fd_server, MAX_LISTEN) < 0)
		throw std::runtime_error("listen error");
	make_socket_non_blocking(_fd_server);
}

int	Socket::accept_client(int server_fd)
{
	int	new_client;
	new_client = accept(server_fd, (struct sockaddr *)NULL, NULL);
	if (new_client == -1)
	{
		std::cerr << "accept new client error" << std::endl;
		return (-1);
	}
	make_socket_non_blocking(new_client);
	return (new_client);
}

void	Socket::make_socket_non_blocking(int socket)
{
	int flags;

	flags = fcntl(socket, F_GETFL, 0);
	if (flags == -1)
	{
		std::cerr << "flag recuperation for fd: " << socket << " error";
		exit (EXIT_FAILURE);
	}
	flags |= O_NONBLOCK;
	if (fcntl(socket, F_SETFL, flags) == -1)
	{
		std::cerr << "Set flag for fd: " << socket << " error";
		exit (EXIT_FAILURE);
	}
}
