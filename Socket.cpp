 #include "Socket.hpp"
#include <fcntl.h>
#include <netinet/in.h>
#include <strings.h>

 Socket::Socket(int port){
	 _port = port;
	 _fd_server = -1;
 }

 Socket::Socket(){
	 _port = 0;
	 _fd_server = -1;
 }

 Socket::~Socket(){
 }

 Socket::Socket(const Socket &copy){
 	*this = copy;
 }

 Socket & Socket::operator=(const Socket &copy){
 	if (&copy != this)
 	{
		_fd_server = copy._fd_server;
		_port = copy._port;
 	}
 	return (*this);
 } 

int	Socket::get_fdServer()
{
	return(_fd_server);
}

int	Socket::get_port()
{
	return(_port);
}

void	Socket::allow_socket_server()
{
	init_socket();
	init_struct_server();
	socket_setoptions();
	bind_socket();
	listen_socket();
}

void	Socket::init_socket()
{
	_fd_server = socket(AF_INET, SOCK_STREAM, 0);
	if (_fd_server < 0)
	{
		perror("creating socket error\n");
		exit (1);
	}
}

void	Socket::init_struct_server()
{
	bzero(&_server_addr, sizeof(struct sockaddr_in));
	_server_addr.sin_family = AF_INET;
	_server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	_server_addr.sin_port = htons(_port);

}

void	Socket::socket_setoptions()
{
	int	option = 1;

	if (setsockopt(_port, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(int)) < 0)
	{
		perror("bind socket to the right port error\n");
		exit (1);
	}
}

void	Socket::bind_socket()
{
	if (bind(_port, (struct sockaddr *)&_server_addr, sizeof(_server_addr)) < 0)
	{
		perror("bind socket to the right port error\n");
		exit (1);
	}
}

void	Socket::listen_socket()
{
	if (listen(_port, 10) < 0)
	{
		perror("listen error\n");
		exit (1);
	}
}

int	Socket::accept_client(int server_fd)
{
	int	new_client;
	std::cout << "Ajoute d'un client dans la pool de fd: ";
	new_client = accept(server_fd, (struct sockaddr *)NULL, NULL);
	if (new_client == -1)
	{
		perror("accept error\n");
		exit (EXIT_FAILURE);
	}
	return (new_client);
}
