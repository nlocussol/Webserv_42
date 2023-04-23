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
	int option = 1;

	_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (_socket < 0)
	{
		perror("creating socket error\n");
		return ;
	}
	bzero(&_serv_addr, sizeof(struct sockaddr_in));
	_serv_addr.sin_family = AF_INET;
	_serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	_serv_addr.sin_port = htons(_port);
	if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(int)) < 0)
	{
		perror("bind socket to the right port error\n");
		return ;
	}
	if (bind(_socket, (struct sockaddr *)&_serv_addr, sizeof(_serv_addr)) < 0)
	{
		perror("bind socket to the right port error\n");
		return ;
	}
	if (listen(_socket, 10) < 0)
	{
		perror("listen error\n");
		return ;
	}
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
			// std::cout << "Sending img\n";
			// fdt = open("img.jpg", O_RDONLY);
			// fstat(fdt, &filestat);
			// sprintf(filesize, "%zd", filestat.st_size);
			// fp = fopen("img.jpg", "r");
			// strcpy(data, "HTTP/1.1 200 OK\r\nContent-Length: ");
			// strcat(data, filesize);
			// strcat(data, "\r\n");
			// strcat(data, "Content-Type: image/*\r\n\r\n");
			// std::cout << data;
			// write(client_fd, data, strlen(data));
			// char *datafile = (char *)malloc(100000);
			// fread(datafile, sizeof(char), filestat.st_size, fp);
			// // fclose(fp);
			// write(client_fd, datafile, filestat.st_size);
			// free(datafile);
		}
		else if (buffer.find("GET / HTTP/1.1") != std::string::npos) {
			Request request(Request::TEXT_HTML);
			sendRequest(request, client_fd);
			/*buffer = "HTTP/1.1 200 OK\r\n\
							Content-Type: text/html\r\n\
							Connection: keep-alive\r\n\
				\r\n<!DOCTYPE html>\
\r\n<html>\
\r\n<body>\
\r\n\
\r\n<h1>My First Mbaboin</h1>\
\r\n\
\r\n<p>My first paragraph.</p>\
\r\n\
<img src=\"img.jpg\">\r\n\
<a href=\"https://www.youtube.com/watch?v=dQw4w9WgXcQ&pp=ygUJcmljayByb2xs/\">Baguette</a>\r\n\
\r\n</body>\
\r\n</html>\r\n";*/
		}
		// send (client_fd, buffer.c_str(), buffer.length(), 0);
		// close (fdimg);
		// close (client_fd);
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
