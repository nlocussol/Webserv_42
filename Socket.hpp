#include "fcntl.h"
#include <netinet/in.h>
#include <unistd.h>
#include <cstdlib>
#include <iostream>

class Socket
 {
 	 public:
 		Socket(int port);
 		~Socket();
 		Socket (const Socket &copy);
 		Socket &operator=(const Socket&);

		void	allow_socket_server();
		int		get_fdServer();
		int		get_port();
 	 private:
		void	init_socket();
		void	init_struct_server();
		void	socket_setoptions();
		void	bind_socket();
		void	listen_socket();

		int		_fd_server;
		int		_port;
		struct sockaddr_in _server_addr;
 		
 };
