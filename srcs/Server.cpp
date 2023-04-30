#include "../inc/Server.hpp"
#include "../inc/Response.hpp"
#include "../inc/webserv.hpp"
#include <cstdlib>
#include <unistd.h>

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
		_epoll.add_fd_to_pool(new_client);
		_fd.add_new_client(new_client, event.data.fd);
	}
	else
	{
		int	server;
		server = _fd.find_matching_server(event.data.fd);	
		readRequest(event.data.fd);/*, server*/
		Request request(_buffer);
		std::cout << "Request-----\n" << _buffer;
		request.parseRequest(_servers, server);
		request.findRequestType();
		if (request._statusCode != UNSUPPORTED_REQUEST) {
			request.findRequestSubType();
			switch (request._requestType) {
				case GET_REQUEST:
					request._statusCode = handleGetRequest(server);
					break;
				case POST_REQUEST:
					request._statusCode = handlePostRequest(server);
					break;
				case DELETE_REQUEST:
					request._statusCode = handleDeleteRequest(server);
					break;
			}
		}
		Response response;
		response.buildResponse(request, _filePath);
		// std::cout << "Response------\n" << response.getCompleteResponse();
		sendResponse(response, event.data.fd);
		_filePath.clear();
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

int Server::handleGetRequest(int server)
{
	//Parse buffer to find file path such as : GET /file.extension HTTP1.1
	_filePath = _buffer.substr(_buffer.find_first_of(" ") + 1);
	_filePath = _filePath.substr(1, _filePath.find_first_of(" ") - 1);
	/*
	 * regrade si c'est un cgi et le lance au besoin.
	 * décommenter ces fonctions quand parsing sur cgi sera fait
	 * pour l'instant, renvoie un fd mais peu renvoyer une string au besoin
	*/
	if (!_filePath.empty() && is_cgi(_servers.serv[server], _filePath) == true)
		handle_cgi(_servers.serv[server], _filePath);
	MULTIMAP::iterator itPathRoot, itPathIndex;
	itPathRoot = _servers.serv[server].conf.find("root");
	itPathIndex = _servers.serv[server].conf.find("index");
	if (_filePath == "") {
		_filePath = itPathRoot->second + itPathIndex->second;
		return 200;
	}
	_filePath.insert(0, itPathRoot->second + "/");
	// Return 404 Not Found if the file does not exist
	if (access(_filePath.c_str(), F_OK))
		return 404;
	// Return 403 Forbidden if the file is not readable
	if (access(_filePath.c_str(), R_OK))
		return 403;
	return 200;
}

int Server::handlePostRequest(int server)
{
	// Parse buffer to find file path such as : POST /file.extension HTTP1.1
	_filePath = _buffer.substr(_buffer.find_first_of(" ") + 1);
	_filePath = _filePath.substr(1, _filePath.find_first_of(" ") - 1);
	MULTIMAP::iterator itPathRoot;
	itPathRoot = _servers.serv[server].conf.find("root");
	_filePath.insert(0, itPathRoot->second);
	// Return 403 Forbidden is the file is core of our project
	if (isFileProtected())
		return 403;

	// Check if the file already exists, if it does try to delete it
	std::ifstream infile;
	// Also need to test if directory is writable, but j'ai la flemme là
	infile.open(_filePath.c_str());
	if (infile) {
		if (access(_filePath.c_str(), F_OK | R_OK))
			return 403;
		else {
	// Need to add proper error code return, but file should be deletable since we access it upward
			if (std::remove(_filePath.c_str()))
				return 0;
		}
	}
	// Create file with the same name and fill it with body of POST request
	size_t bodyBegin = _buffer.find("\r\n\r\n");
	std::string bodyContent = _buffer.substr(bodyBegin + 4);
	std::ofstream outfile(_filePath.c_str());
	outfile << bodyContent;
	outfile.close();
	return 200;
}

int Server::handleDeleteRequest(int server)
{

	return 200;
}

bool Server::isFileProtected() const
{
	if (_filePath.find("utils/BIDEN_BLAST.mp4") != std::string::npos)
		return true;
	if (_filePath.find("utils/index.html") != std::string::npos)
		return true;
	if (_filePath.find("utils/favicon.ico") != std::string::npos)
		return true;
	if (_filePath.find("utils/img.jpg") != std::string::npos)
		return true;
	return false;
}

void Server::sendResponse(Response& response, int client_fd) 
{
	send(client_fd, response.getCompleteResponse().c_str(), response.getCompleteResponse().length(), 0);
}

std::string Server::getBuffer(void) const
{
	return _buffer;
}
