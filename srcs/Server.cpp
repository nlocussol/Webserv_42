#include "../inc/Server.hpp"
#include <cstdlib>

bool Server::_running = true;

Server::Server(data servers) : _socket(), _epoll(), _fd() 
{
	signal(SIGINT, handle_sigint);
	_statusCode = 0;
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

Server::Server(std::multimap<std::string, std::string>& config)
{
	_config = config;
	_running = true;
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
	while (_running)
	{
		struct epoll_event event;
		epoll_wait(_epoll.get_fd_epoll(), &event, MAX_EVENT, -1);
		manage_epoll_wait(event);
	}
}

void Server::manage_epoll_wait(struct epoll_event &event)
{
	if (((event.events & EPOLLERR) || (event.events & EPOLLHUP) || (!(event.events & EPOLLIN))))
	{
		perror("fd wrong signal\n");
		exit (EXIT_FAILURE);
	}
	if (_fd.is_server(event.data.fd) == true)
	{
		int	new_client = _socket.accept_client(event.data.fd);
		_epoll.add_fd_to_pool(new_client);
		_fd.add_new_client(new_client, event.data.fd);
	}
	else
	{
		int	server;
		server = _fd.find_matching_server(event.data.fd);	
		readRequest(event.data.fd);/*, server*/
		std::cout << "Request-----\n" << _buffer;
		int requestType = findRequestType();
		int requestSubType = findRequestSubType();
		std::cout << requestType << '\n';
		switch (requestType) {
			case GET_REQUEST :
				_statusCode = handleGetRequest(server);
				break;
			case POST_REQUEST :
				break;
			case DELETE_REQUEST :
				break;
			default :
				break;
		}
		Request request(requestType, requestSubType, _statusCode, _filePath);
		// std::cout << "Response------\n" << request.getBuffer();
		sendRequest(request, event.data.fd);
		_filePath.clear();
	}
}

void Server::readRequest(int epoll_fd)
{
	char buff[BUFFER_SIZE];
	std::memset(buff, 0, BUFFER_SIZE);
	recv(epoll_fd, buff, BUFFER_SIZE - 1, 0);
	_buffer = buff;
	std::memset(buff, 0, BUFFER_SIZE);
}

int Server::findRequestType()
{
	if (_buffer.find("GET") != std::string::npos)
		return GET_REQUEST;
	if (_buffer.find("POST") != std::string::npos)
		return POST_REQUEST;
	if (_buffer.find("DELETE") != std::string::npos)
		return DELETE_REQUEST;
	return UNSUPPORTED_REQUEST;
}

int Server::findRequestSubType()
{
	if (_buffer.find("Accept: text") != std::string::npos) 
		return TEXT;
	if (_buffer.find("Accept: image") != std::string::npos) 
		return IMAGE;
	return 1;
}

int Server::handleGetRequest(int server)
{
	_filePath = _buffer.substr(_buffer.find_first_of(" ") + 1);
	_filePath = _filePath.substr(1, _filePath.find_first_of(" ") - 1);
	MULTIMAP::iterator itPathRoot, itPathIndex;
	itPathRoot = _servers.serv[server].conf.find("root");
	itPathIndex = _servers.serv[server].conf.find("index");
	if (_filePath == "") {
		_filePath = itPathRoot->second + itPathIndex->second;
		return 200;
	}
	_filePath.insert(0, itPathRoot->second + "/");
	if (access(_filePath.c_str(), F_OK))
	{
		// perror("File does not exist\n");
		return 404;
	}
	if (access(_filePath.c_str(), R_OK))
	{
		// perror("File can't be red\n");
		return 403;
	}
	return 200;
}

void Server::sendRequest(Request& request, int client_fd) 
{
	send(client_fd, request.getBuffer().c_str(), request.getBuffer().length(), 0);
}
