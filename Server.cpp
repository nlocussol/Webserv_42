#include "Server.hpp"
#include <cstdlib>
#include <cstring>
#include <string>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>


bool Server::_running = true;

Server::Server() : _socket(16161)
{
	signal(SIGINT, handle_sigint);
	_port = 16161;
	_nb_server = 1;
}

Server::~Server()
{
}

Server::Server(const Server& other)
{
	*this = other;
}

Server& Server::operator=(const Server& other)
{
	if (this == &other)
		return *this;
	*this = other;
	return *this;
}

Server::Server(std::multimap<std::string, std::string>& config)
{
	_config = config;
	_running = true;
	_port = 16162;
}

void Server::setSocket(void)
{
	int	server;
	_epoll.create_epoll();
	for (int i = 0; i < _nb_server; i++)
	{
		_socket.allow_socket_server(_port);
		server = _socket.get_fdServer();
		_fd.set_fd_server(server);
		_epoll.add_fd_to_pool(server);
	}
}

void Server::runServer(void)
{
/*	int client_fd;
	int n;
	std::string buffer;
	struct stat filestat;
	FILE *fp;
	int fdt;
	char filesize[7];
	char data[100000]// , datafile[100000] ;
	std::memset(data, 0, 100000);
	while (1)
	{
		buffer = readFd(&client_fd);
		std::cout << buffer << '\n';
		if (buffer.find("GET") != std::string::npos && buffer.find("Accept: image/") != std::string::npos) {
			Request request(2);
			sendRequest(request, client_fd);
		}
		else if (buffer.find("GET / HTTP/1.1") != std::string::npos) {
			Request request(Request::TEXT_HTML);
			sendRequest(request, client_fd);
		}
	}
*/
	// std::string buffer;
	while (_running)
	{
		struct epoll_event event;
		epoll_wait(_epoll.get_fd_epoll(), &event, MAX_EVENT, -1);
		manage_epoll_wait(event);
		// buffer = readFd(&client_fd);
		// std::cout << buffer << '\n';
		// if (buffer.find("GET") != std::string::npos && buffer.find("Accept: image/") != std::string::npos) {
		// 	Request request(2);
		// 	sendRequest(request, client_fd);
		// }
		// else if (buffer.find("GET / HTTP/1.1") != std::string::npos) {
		// 	Request request(Request::TEXT_HTML);
		// 	sendRequest(request, client_fd);
		}
	}
// }

// std::string Server::readFd(int* client_fd)
// {
// 	std::string buffer;
// 	char buffer_read[256];
// 	memset(buffer_read, 0, 256);
// 	printf("Waiting for a connection on port %d\n", _port);
// 	*client_fd = accept(_socket, (struct sockaddr *)NULL, NULL);
// 	int byte_r;
// 	while ((byte_r = read(*client_fd, buffer_read, 255)) > 0 ) {
// 		if (buffer_read[byte_r - 1] == '\n')
// 			break ;
// 		buffer += buffer_read;
// 		memset(buffer_read, 0, 256);
// 	}
// 	if (byte_r < 0) {
// 		perror("read error\n");
// 		return NULL;
// 	}
// 	return buffer;
// }

void Server::sendRequest(Request& request, int client_fd) 
{
	send(client_fd, request.getBuffer().c_str(), request.getBuffer().length(), 0);
}

void Server::manage_epoll_wait(struct epoll_event &event)
{
	if (((event.events & EPOLLERR) || (event.events & EPOLLHUP) || (!(event.events & EPOLLIN))))
	{
		perror("fd wrong signal\n");
		exit (EXIT_FAILURE);
	}
	if (_fd.is_server(event.data.fd) == true)
	{
		int	new_client = _socket.accept_client(event.data.fd);
		_epoll.add_fd_to_pool(new_client);
		_fd.add_new_client(new_client, event.data.fd);
	}
	else
	{
		int	server;
		server = _fd.find_matching_server(event.data.fd);	
		readRequest(event.data.fd);/*, server*/
		Request request(1);
		sendRequest(request, event.data.fd);
	}
}

void Server::readRequest(int epoll_fd)
{
	char buff[BUFFER_SIZE];
	std::memset(buff, 0, BUFFER_SIZE);
	recv(epoll_fd, buff, BUFFER_SIZE - 1, 0);
	std::cout << buff;
}
