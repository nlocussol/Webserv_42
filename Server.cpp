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

Server::Server()
{
	_running = true;
	_port = 16161;
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
	int _socket = socket(AF_INET, SOCK_STREAM, 0);
	if (_socket < 0) {
		perror("Error while creating socket\n");
		exit(EXIT_FAILURE);
	}
	struct sockaddr_in serv_addr;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(16161);
	int option = 1;
	if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(int)) < 0) {
		perror("Error\n");
		exit(EXIT_FAILURE);
	}
	if (bind(_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		perror("Error binding the socket\n");
		exit(EXIT_FAILURE);
	}
	if (listen(_socket, 10) < 0) {
		perror("Error listen\n");
		exit(EXIT_FAILURE);
	}
}

void Server::runServer(void)
{
	char defaultHeader[] = "HTTP/1.1 200 OK\r\n"; 
	char basicHTMLPage[] = "Content-Type: text/html\r\nContent-Length:100\r\n<!DOCTYPE html>\
\r\n<html>\
\r\n<body>\
\r\n\
\r\n<h1>My First Heading</h1>\
\r\n\
\r\n<p>My first paragraph.</p>\
\r\n\
<img src=\"img.jpg\">\r\n\
<a href=\"https://www.youtube.com/watch?v=dQw4w9WgXcQ&pp=ygUJcmljayByb2xs/\">Baguette</a>\r\n\
\r\n</body>\
\r\n</html>\r\n";


	int client_fd;
	int byte_r;
	char buff[2048];
	//sig handler need to change running to false
	std::cout << "Waiting for connexion on port " << _port << '\n';
	while (_running) {
		client_fd = accept(_socket, (struct sockaddr *)NULL, NULL);
		std::memset(buff, 0, 2048);
		while ((byte_r = read(client_fd, buff, 2047) > 0 )) {
			if (buff[byte_r - 1] == '\n')
				break;
			std::memset(buff, 0, 256);
		}
		if (byte_r < 0) {
			perror("Error reading\n");
			exit(EXIT_FAILURE);
		}
		sendToClient(defaultHeader, basicHTMLPage, client_fd);
	}
}

void Server::sendToClient(std::string header, std::string buffer, int client_fd)
{
	std::string toSend = header + buffer;
	send(client_fd, toSend.c_str(), toSend.length(), 0);
}

void Server::launchServer(void)
{

}
