#ifndef SOCKET_HPP
# define SOCKET_HPP

# include "fcntl.h"
# include <netinet/in.h>
# include <unistd.h>
# include <cstdlib>
# include <iostream>
# include <vector>

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

 	 private:
		void	make_socket_non_blocking(int socket);
		int		_fd_server;
		int		_port;
		struct sockaddr_in _server_addr;
		std::vector<std::string> msg;
 		
 };
#endif
