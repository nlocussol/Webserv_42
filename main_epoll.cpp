#include <cstdio>
#include <fcntl.h>
#include <cstdlib>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include "fcntl.h"
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <sys/epoll.h>
#include <stdlib.h>

std::string setRequest()
{
	std::string ret = "HTTP/1.1 200 OK\r\n\r\n<!DOCTYPE html>\
		\r\n<html>\
		\r\n<body>\
		\r\n\
		\r\n<h1>My First Heading</h1>\
		\r\n\
		\r\n<p>My first paragraph.</p>\
		\r\n\
		<a href=\"https://www.youtube.com/watch?v=dQw4w9WgXcQ&pp=ygUJcmljayByb2xs/\">Important</a>\r\n\
		\r\n</body>\
		\r\n</html>\r\n";
	return (ret);
}

void	handle_connection(int client_fd)
{
	int n;
	std::string buff;
	char recvline[25600];
	
	memset(recvline, 0, 25600);
	while ((n = read(client_fd, recvline, 25599)) > 0 )
	{
		std::cout << "Packet received:\n" << recvline << std::endl;
		if (recvline[n - 1] == '\n')
			break ;
		memset(recvline, 0, 256);
	}
	if (n < 0)
	{
		perror("read error\n");
		exit (EXIT_FAILURE);
	}
	buff = setRequest();
	if (send (client_fd, buff.c_str(), buff.length(), 0) == -1 )
	{
		perror("send error\n");
		exit (EXIT_FAILURE);
	}
}

int	initSocket()
{
	int	sock_listen;
	int	port = 16161;
	int option = 1;
	struct sockaddr_in	serv_addr;

	sock_listen = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_listen < 0)
	{
		perror("creating socket error\n");
		exit (1);
	}
	bzero(&serv_addr, sizeof(struct sockaddr_in));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(port);
	if (setsockopt(sock_listen, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(int)) < 0)
	{
		perror("bind socket to the right port error\n");
		exit (1);
	}
	if (bind(sock_listen, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		perror("bind socket to the right port error\n");
		exit (1);
	}
	if (listen(sock_listen, 10) < 0)
	{
		perror("listen error\n");
		exit (1);
	}
	return (sock_listen);
}

int main (void)
{
//initialise le socket server
	int	sock_listen, client_fd;
	sock_listen = initSocket();
// initialise le fd de epoll
	struct epoll_event event;
	int	epfd = epoll_create1(0);
	if (epfd == -1){
		std::cerr << "Failed to create epoll fd" << std::endl;
		exit(EXIT_FAILURE);
	}
	event.events = EPOLLIN;
	event.data.fd = epfd;
	epoll_ctl(epfd, EPOLL_CTL_ADD, sock_listen, &event);
//récupère le fd du server et l'ajoute a la pool de fd d'epoll
	while (1){
		struct epoll_event event;
		int	event_count;
		//attend un event dans la pool de fd
		event_count = epoll_wait(epfd, &event, 10, 300000);
		if (event_count == -1)
		{
			perror("epoll wait error\n");
			exit (EXIT_FAILURE);
		}
		if (event_count){
		//si event correspond au fd du server alors ajouter un fd client a la pool
			if (event.data.fd == epfd){
				std::cout << "Ajoute d'un client dans la pool de fd: ";
				client_fd = accept(sock_listen, (struct sockaddr *)NULL, NULL);
				if (client_fd == -1)
				{
					perror("accept error\n");
					exit (EXIT_FAILURE);
				}
				event.events = EPOLLIN;
				event.data.fd = client_fd;
				std::cout << client_fd << std::endl;
				if (epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &event) == -1)
				{
					perror("epoll ctl error\n");
					exit (EXIT_FAILURE);
				}
			}
		//si event correspond a un client, envoyer un paquet à ce dernier
			else if (event.events & EPOLLIN) {
				std::cout << "Envoie d'un packet au fd: " << event.data.fd <<  std::endl;
				handle_connection(event.data.fd);
			}
			else{
				std::cout << "error" << std::endl;
			}
		}
	}
	close (epfd);
	return (0);
}
