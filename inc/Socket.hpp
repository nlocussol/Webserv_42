#ifndef SOCKET_HPP
# define SOCKET_HPP

# include "fcntl.h"
# include <netinet/in.h>
# include <unistd.h>
# include <cstdlib>
# include <iostream>
# include <vector>

#define MAX_LISTEN 1000

class Socket
 {
 	 public:
 		Socket(int port);
 		Socket();
 		~Socket();
 		Socket (const Socket &copy);
 		Socket &operator=(const Socket&);

		void	allow_socket_server(int);
		int		get_fdServer();
		int		get_port();
		int		accept_client(int server_fd);
		void	make_socket_non_blocking(int socket);

	 private:
		int		_fd_server;
		int		_port;
		struct sockaddr_in _server_addr;
 };
#endif
