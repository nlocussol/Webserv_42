#include "Server.hpp"
#include <cstdlib>
#include <cstring>
#include <map>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "Request.hpp"

Server::Server()
{
	_running = true;
	_port = 16162;
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
	_port = 16161;
}

void Server::setSocket(void)
{
	_socket.allow_socket_server();
}

void Server::runServer(void)
{
	int client_fd;
	int n;
	std::string buffer;
	struct stat filestat;
	FILE *fp;
	int fdt;
	char filesize[7];
	char data[100000]/* , datafile[100000] */;
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

std::string Server::readFd(int* client_fd)
{
	std::string buffer;
	char buffer_read[256];
	memset(buffer_read, 0, 256);
	printf("Waiting for a connection on port %d\n", _port);
	*client_fd = accept(_socket, (struct sockaddr *)NULL, NULL);
	int byte_r;
	while ((byte_r = read(*client_fd, buffer_read, 255)) > 0 ) {
		if (buffer_read[byte_r - 1] == '\n')
			break ;
		buffer += buffer_read;
		memset(buffer_read, 0, 256);
	}
	if (byte_r < 0) {
		perror("read error\n");
		return NULL;
	}
	return buffer;
}

void Server::sendRequest(Request& request, int client_fd) 
{
	send(client_fd, request.getBuffer().c_str(), request.getBuffer().length(), 0);
}

void Server::sendToClient(std::string header, std::string buffer, int client_fd)
{
	(void)header;
	(void)buffer;
	(void)client_fd;
}

void Server::launchServer(void)
{

}
