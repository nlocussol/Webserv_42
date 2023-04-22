#include <cstdio>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>
#include <stdio.h>

int main (int ac, char **av)
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
	if (ac <= 1)
	{
		perror("wrong number of args\n");
		return (1);
	}

	char recvline[256];
	char buff[256];
	while (1)
	{
		printf("Waiting for a connection on port %d\n", port);
		client_fd = accept(sock_listen, (struct sockaddr *)NULL, NULL);
		memset(recvline, 0, 256);
		while ((n = read(client_fd, recvline, 255)) > 0 )
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
		snprintf((char *)buff, sizeof(buff), "HTTP/1.1 200 OK\r\n\r\n<!DOCTYPE html>\
\r\n<html>\
\r\n<body>\
\r\n\
\r\n<h1>My First Heading</h1>\
\r\n\
\r\n<p>My first paragraph.</p>\
\r\n\
<img src=\"pic_trulli.jpg\">\r\n\
\r\n</body>\
\r\n</html>");
		send (client_fd, buff, strlen(buff), 0);
		snprintf((char *)buff, sizeof(buff), "GET .pic_trulli.png HTTP 1.1\r\n");
		send (client_fd, buff, strlen(buff), 0);
		close (client_fd);
	}
	return (0);
}
