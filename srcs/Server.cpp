#include "../inc/Server.hpp"
#include "../inc/Response.hpp"
#include "../inc/webserv.hpp"

bool Server::_running = true;

Server::Server(data servers) : _socket(), _epoll() 
{
	signal(SIGINT, handle_sigint);
	_nb_server = servers.v_serv.size();
	_running = true;
	_servers = servers;
	_posBuffer = 0;
}

Server::~Server(){
	for (multimap<int, int>::iterator it = _serversId.begin(); it != _serversId.end(); it++)
	{
		if (it->first > 0)
			close (it->first);
	}
	vector<Client>::iterator end = _clients.end();

	for (vector<Client>::iterator client = _clients.begin(); client != end; client++){
		if (client->getFdClient() > 0){
			std::cerr << "Client " << client->getFdClient() << " connection closed\n";
			close (client->getFdClient());
			if (client->getCgiFd() != -1)
				close (client->getCgiFd());
		}
	}
	int wstatus = 0;
	for (map<int, int>::iterator it = _pid.begin(); it != _pid.end(); it++){
		if (waitpid(it->second, &wstatus, WNOHANG) != 0)
			continue;
		kill(it->second, SIGKILL);
	}
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
			std::cerr << "Server " << i << " listen on port " << port << std::endl;
			_socket.allow_socket_server(port);
			server = _socket.get_fdServer();
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
	int	i;

	while (_running)
	{
		nb_event = epoll_wait(_epoll.get_fd_epoll(), event, MAX_EVENT, -1);
		for(i = 0; i < nb_event; i++){
			manage_epoll_wait(event[i]);
		}
	}
}

void Server::manage_epoll_wait(struct epoll_event &event)
{
	if ((event.events & EPOLLERR) || (event.events & EPOLLHUP) || !(event.events & EPOLLIN))
	{
		removeClient(event.data.fd);
		return ;
	}
	if (isServer(event.data.fd) == true)
	{
		acceptNewClient(event.data.fd);	
	}
	else
	{
		int	serverId;
		if ((serverId = findMatchingServer(event.data.fd)) == -1)
		{
			std::cerr << "No matching server for " << event.data.fd << " founded" << std::endl;
		}
		// return false if recv don't read the entire fd or if the connection is stopped
		if (readRequest(event.data.fd) == false)
			return;
		// find matching client in the clients vector	
		vector<Client>::iterator client;
		vector<Client>::iterator end = _clients.end();

		for (client = _clients.begin(); client != end; client++){
			if (client->getCgiFd() == event.data.fd){
				event.data.fd = client->getFdClient();
				close (client->getCgiFd());
				client->setCgiFd(-1);
				break ;
			}
			if (client->getFdClient() == event.data.fd){
				break ;
			}
		}
		_buffer.assign(client->_buffer, 0, client->_buffer.length());
		Request request(_buffer, _servers, serverId, event.data.fd, client->_cgiOver, _pid);
		//std::cerr << "Request-----\n" << _buffer;
		try {
			request.parseRequest();
		}
		catch (int fd){
			if (fd != -1)
			{
				_socket.make_socket_non_blocking(fd);
				client->setCgiFd(fd);
				_epoll.add_fd_to_pool(fd);
			}
			return ;
		}
		// cerr << request;
		Response response(_servers.v_serv[serverId], request._filePath);
		response.buildResponse(request);
		//std::cerr << "Response------\n" << response.getCompleteResponse();
		sendResponse(response, event.data.fd);
		client->resetBuffer();
	}
}

bool	Server::readRequest(int epoll_fd)
{
	int	readReturn =  0;
	vector<Client>::iterator client;
	vector<Client>::iterator end = _clients.end();

	for (client = _clients.begin(); client != end; client++){
		if (client->getFdClient() == epoll_fd || client->getCgiFd() == epoll_fd){
			break ;
		}
	}
	if (client == end)
		std::cerr << "No matching client found in the fd pool" << std::endl;
	readReturn = client->readFromFd();
	//FINISH : recv read less than buffer size : the request can be interpreted
	if (readReturn == FINISH)
		return (true);

	//DEL : recv return 0 : the client close the connection
	if (readReturn == DEL || readReturn == -1)
		removeClient(client->getFdClient());
	return (false);
}

void Server::sendResponse(Response& response, int client_fd) 
{
	if (send(client_fd, response.getCompleteResponse().c_str(),	response.getCompleteResponse().length(), 0)
		== -1)
	{
		removeClient(client_fd);
	}
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
		std::cerr << "Accept " << fdFromEpoll << " error" << endl;
		return ;
	}
	_epoll.add_fd_to_pool(new_client);
	idServer = _serversId.find(fdFromEpoll)->second;

	Client newClient(new_client, idServer);
	_clients.push_back(newClient);
	// std::cerr << "New client: " << new_client << std::endl;
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
	{
		if (it->getFdClient() == fdClient || it->getCgiFd() == fdClient)
			return (it->getIdServer());
	}
	return (-1);
}

void	Server::removeClient(int fdClient)
{
	vector<Client>::iterator client;
	vector<Client>::iterator end = _clients.end();

	for (client = _clients.begin(); client != end; client++){
		if (client->getFdClient() == fdClient){
			break ;
		}
	}
	if (client == end)
		std::cerr << "No matching client found in the fd pool" << std::endl;
	// If the client get loop in cgi
	int wstatus = 0;
	for (map<int, int>::iterator it = _pid.begin(); it != _pid.end(); it++){
		if (it->first == client->getFdClient())
		{
			if (waitpid(it->second, &wstatus, WNOHANG) != 0)
				continue;
			kill(it->second, SIGKILL);
		}
	}
	// std::cerr << "Client " << fdClient << " connection closed\n";
	if (client->getCgiFd() != -1)
	{
		_epoll.del_fd_from_pool(client->getCgiFd());
		close(client->getCgiFd());
	}
	_clients.erase(client);
	_epoll.del_fd_from_pool(fdClient);
	close(fdClient);
}
