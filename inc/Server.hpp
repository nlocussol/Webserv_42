#ifndef SERVER_HPP_
#define SERVER_HPP_

#include <map>
#include <netinet/in.h>
#include <string>
#include <sys/epoll.h>
#include <cstring>
#include <signal.h>
#include "Request.hpp"
#include "Response.hpp"
#include "Epoll.hpp"
#include "Socket.hpp"
#include "Client.hpp"
#include "parsing.hpp"

#define MAX_EVENT 1000

class Server { 
	public:
	 Server(data);
	 ~Server();
	 Server(const Server&);
	 Server& operator=(const Server&);
	 void	setSocket(void);
	 void	runServer(void);
	 bool	readRequest(int epoll_fd);
	 int handlePostRequest(int);
	 bool isFileProtected() const;
	 int handleDeleteRequest(int);
	 void	sendResponse(Response&, int);
	 void	manage_epoll_wait(struct epoll_event&);
	 static bool _running;

	private:
	 bool	isServer(int fdFromEpoll);
	 void	acceptNewClient(int fdFromEpoll);
	 void	removeClient(int fdClient);
	 int	findMatchingServer(int fdClient);
	 Socket _socket;
	 Epoll _epoll;
	 int _nb_server;
	 std::string _buffer;
	 size_t _recvReturn;
	 size_t _posBuffer;
	 data _servers;
	 multimap<int, int> _serversId;
	 vector<Client>	_clients;
	 std::map<int, int>	_pid;
 } ;

void	handle_sigint(int signum);

#endif // SERVER_HPP_
