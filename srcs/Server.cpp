#include "../inc/Server.hpp"
#include "../inc/Response.hpp"
#include "../inc/webserv.hpp"
#include <cstdlib>
#include <unistd.h>

bool Server::_running = true;

Server::Server(data servers) : _socket(), _epoll(), _fd() 
{
	signal(SIGINT, handle_sigint);
	_nb_server = servers.serv.size();
	_running = true;
	_servers = servers;
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

void Server::setSocket(void)
{
	int	server;
	int	port;
	MULTIMAP::iterator it;

	_epoll.create_epoll();
	for (int i = 0; i < _nb_server; i++)
	{
		it  = _servers.serv[i].conf.find("listen");
		while (it != _servers.serv[i].conf.end())
		{
			port = atoi(it->second.c_str());
			std::cout << "Server " << i << " listen on port " << port << std::endl;
			_socket.allow_socket_server(port);
			server = _socket.get_fdServer();
			_fd.set_fd_servers(server, i);
			_epoll.add_fd_to_pool(server);

			_servers.serv[i].conf.erase(it);
			it  = _servers.serv[i].conf.find("listen");
		}
	}
}

void Server::runServer(void)
{
	int	nb_event;
	struct epoll_event event[MAX_EVENT];
	while (_running)
	{
		nb_event = epoll_wait(_epoll.get_fd_epoll(), event, MAX_EVENT, -1);
		for(int i = 0; i < nb_event; i++){
			manage_epoll_wait(event[i]);
		}
	}
}

void Server::manage_epoll_wait(struct epoll_event &event)
{
	if (((event.events & EPOLLERR) || (event.events & EPOLLHUP) || (!(event.events & EPOLLIN))))
	{
		perror("fd wrong signal\n");
		close (event.data.fd);
	}
	if (_fd.is_server(event.data.fd) == true)
	{
		int	new_client = _socket.accept_client(event.data.fd);
		if (new_client == -1)
			return ;
		_epoll.add_fd_to_pool(new_client);
		_fd.add_new_client(new_client, event.data.fd);
	}
	else
	{
		int	serverFd;
		serverFd = _fd.find_matching_server(event.data.fd);	
		readRequest(event.data.fd);/*, server*/
		Request request(_buffer, _servers, serverFd);
		// std::cout << "Request-----\n" << _buffer;
		request.parseRequest(_servers, serverFd);
		Response response;
		response.buildResponse(request, request._filePath);
		// std::cout << "Response------\n" << response.getCompleteResponse();
		sendResponse(response, event.data.fd);
	}
}

void Server::readRequest(int epoll_fd)
{
	char buff[BUFFER_SIZE];
	std::memset(buff, 0, BUFFER_SIZE);
	//Armand -> condition a enveler si probleme, doit prevenir de l'ouverture de plus de 1024 fd
	if (recv(epoll_fd, buff, BUFFER_SIZE - 1, 0) == 0)
		close(epoll_fd);
	_buffer = buff;
	std::memset(buff, 0, BUFFER_SIZE);
}

void Server::sendResponse(Response& response, int client_fd) 
{
	send(client_fd, response.getCompleteResponse().c_str(), response.getCompleteResponse().length(), 0);
}
