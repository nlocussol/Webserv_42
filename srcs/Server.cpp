#include "../inc/Server.hpp"
#include "../inc/Response.hpp"
#include "../inc/webserv.hpp"
#include <cstdlib>
#include <cstring>
#include <unistd.h>

bool Server::_running = true;

Server::Server(data servers) : _socket(), _epoll() 
{
	signal(SIGINT, handle_sigint);
	_nb_server = servers.v_serv.size();
	_running = true;
	_servers = servers;
	_posBuffer = 0;
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
		it  = _servers.v_serv[i].conf_serv.find("listen");
		while (it != _servers.v_serv[i].conf_serv.end())
		{
			port = atoi(it->second.c_str());
			std::cout << "Server " << i << " listen on port " << port << std::endl;
			_socket.allow_socket_server(port);
			server = _socket.get_fdServer();
			//_fd.set_fd_servers(server, i);
			_serversId.insert(make_pair(server, i));
			_epoll.add_fd_to_pool(server);

			_servers.v_serv[i].conf_serv.erase(it);
			it  = _servers.v_serv[i].conf_serv.find("listen");
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
	if ((event.events & EPOLLERR) || (event.events & EPOLLHUP) )
	{
		std::cerr << "fd wrong signal" << std::endl;
		close (event.data.fd);
	}
	if (isServer(event.data.fd) == true)
	{
		acceptNewClient(event.data.fd);	
	}
	else
	{
		if (!(event.events & EPOLLIN))
			return ;
		int	serverFd;
		if ((serverFd = findMatchingServer(event.data.fd)) == -1)
		{
			std::cerr << "No matching server for " << event.data.fd << " founded" << std::endl;
		}

		// Need to fix read so we don't send 400 bad request perma
		readRequest(event.data.fd);/*, server*/
		Request request(_buffer, _servers, serverFd);
		// std::cout << "Request-----\n" << _buffer;
		request.parseRequest(_servers, serverFd);
		Response response(_servers.v_serv[serverFd]);
		response.buildResponse(request);
		// std::cout << "Response------\n" << response.getCompleteResponse();
		sendResponse(response, event.data.fd);
	}
}

void Server::readRequest(int epoll_fd)
{
	// vector<Client>::iterator client;
	// for (client = _clients.begin(); client != _clients.end(); client++)
	// {
	// 	if ((*client)._fdClient == epoll_fd)
	// 		break ;
	// }
	// if (client == _clients.end())
	// {
	// 	std::cerr << "No matching client in the vector client" << std::endl;
	// 	return ;
	// }
	// (*client).readFromFd();
	// _buffer = (*client).getBuffer();
	char buff[BUFFER_SIZE];
	std::memset(buff, 0, BUFFER_SIZE);
	//Armand -> condition a enveler si probleme, doit prevenir de l'ouverture de plus de 1024 fd
	if (recv(epoll_fd, buff, BUFFER_SIZE - 1, MSG_DONTWAIT) == 0)
		close(epoll_fd);
	_buffer.assign(buff, BUFFER_SIZE);
	// write(1, _buffchar, BUFFER_SIZE);
	// std::memset(buff, 0, BUFFER_SIZE);	
}

void Server::sendResponse(Response& response, int client_fd) 
{
	send(client_fd, response.getCompleteResponse().c_str(), response.getCompleteResponse().length(), 0);
}

/**
 * Accept a new client with the socket class, it will returns the new fd
 * this client fd is added to the epoll pool 
 * a new instance of client class is created with the new client fd and server parameters
 * this instance is added to the clients list (std::vector _clients)
**/

void	Server::acceptNewClient(int fdFromEpoll)
{
	int	idServer;
	int	new_client = _socket.accept_client(fdFromEpoll);
	if (new_client == -1)
	{
		cerr << "Accept " << fdFromEpoll << " error" << endl;
		return ;
	}
	_epoll.add_fd_to_pool(new_client);
	idServer = _serversId.find(fdFromEpoll)->second;

	Client newClient(new_client, fdFromEpoll, idServer);
	_clients.push_back(newClient);
}

/**
 * take the fd return by epoll_wait as parameter
 * return true if the epollFd is a server
 * return false if the epollFd is a client
 **/

bool	Server::isServer(int fdFromEpoll)
{
	std::map<int, int>::iterator check;
  check  = _serversId.find(fdFromEpoll);
  if (check == _serversId.end())
          return (false);
  else
	return (true);
}

/**
 * take the fd return by epoll_wait as parameter
 * (in this case it's a client for sure)
 * the func will iterate on the client list to find a matching fd
 * between the fd given by epoll and a client fd from the list
 * So, when the match is found,
 * it return the ID (not the fd) of the server associates
 * (ID is based on the struct data given by the parsing)
 **/

int	Server::findMatchingServer(int fdClient)
{
	for (vector<Client>::iterator it = _clients.begin(); it != _clients.end(); it++)
		if ((*it).getFdClient() == fdClient)
			return ((*it).getIdServer());
	return (-1);
}
