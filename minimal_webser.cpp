#include <cstdio>
#include <fcntl.h>
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
#include <stdlib.h>

int main (void)
{
	int	sock_listen, client_fd;
	int	port = 16161;
	int option = 1;
	struct sockaddr_in	serv_addr;

	sock_listen = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_listen < 0)
	{
		perror("creating socket error\n");
		return (1);
	}
	bzero(&serv_addr, sizeof(struct sockaddr_in));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(port);
	if (setsockopt(sock_listen, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(int)) < 0)
	{
		perror("bind socket to the right port error\n");
		return (1);
	}
	if (bind(sock_listen, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		perror("bind socket to the right port error\n");
		return (1);
	}
	if (listen(sock_listen, 10) < 0)
	{
		perror("listen error\n");
		return (1);
	}
	int n;
	int fdimg;
	char recvline[25600];
	std::string buff;
	struct stat filestat;
	FILE *fp;
	int fdt;
	char filesize[7];
	char data[100000]/* , datafile[100000] */;
	std::memset(data, 0, 100000);
	while (1)
	{
		printf("Waiting for a connection on port %d\n", port);
		client_fd = accept(sock_listen, (struct sockaddr *)NULL, NULL);
		memset(recvline, 0, 25600);
		while ((n = read(client_fd, recvline, 25599)) > 0 )
		{
			if (recvline[n - 1] == '\n')
				break ;
			memset(recvline, 0, 256);
		}
		if (n < 0)
		{
			perror("read error\n");
			return (1);
		}
		if (!strncmp(recvline, "GET /img.jpg", 12))
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
		else {
			buff = "HTTP/1.1 200 OK\r\n\r\n<!DOCTYPE html>\
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
	return (0);
}
