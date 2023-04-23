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
	char buffer[25600];
	std::string buff;
	struct stat filestat;
	FILE *fp;
	int fdt;
	char filesize[7];
	char data[100000]/* , datafile[100000] */;
	std::memset(data, 0, 100000);
	while (1)
	{
		printf("Waiting for a connection on port %d\n", _port);
		client_fd = accept(_socket, (struct sockaddr *)NULL, NULL);
		memset(buffer, 0, 256);
		while ((n = read(client_fd, buffer, 25500)) > 0 )
		{
			std::cout << buffer << std::endl;
			if (buffer[n - 1] == '\n')
				break ;
			memset(buffer, 0, 25600);
		}
		if (n < 0)
		{
			perror("read error\n");
			return ;
		}
		if (!strncmp(buffer, "GET /img.jpg", 12))
		{
			std::cout << "Sending img\n";
			fdt = open("img.jpg", O_RDONLY);
			fstat(fdt, &filestat);
			sprintf(filesize, "%zd", filestat.st_size);
			fp = fopen("img.jpg", "r");
			strcpy(data, "HTTP/1.1 200 OK\r\nContent-Length: ");
			strcat(data, filesize);
			strcat(data, "\r\n");
			strcat(data, "Content-Type: image/jpeg\r\n\r\n");
			std::cout << data;
			write(client_fd, data, strlen(data));
			char *datafile = (char *)malloc(100000);
			fread(datafile, sizeof(char), filestat.st_size, fp);
			// fclose(fp);
			write(client_fd, datafile, filestat.st_size);
			free(datafile);

		}
		else if (!strncmp(buffer, "GET /favicon.ico", 16)) 
		{
			std::cout << "tg\n";
			buff = "HTTP/1.1 404\r\n\r\n";
		}
		else {
			buff = "HTTP/1.1 200 OK\r\n\
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
\r\n</html>\r\n";
		}
		send (client_fd, buff.c_str(), buff.length(), 0);
		// close (fdimg);
		// close (client_fd);
	}


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
