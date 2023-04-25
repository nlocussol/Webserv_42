#ifndef SERVER_HPP_
#define SERVER_HPP_

#include <map>
#include <netinet/in.h>
#include <string>
#include <sys/epoll.h>
#include "Request.hpp"
#include "Epoll.hpp"
#include "Socket.hpp"
#include "FdManager.hpp"

#define MAX_EVENT 10

class Server { 
	public:
	 Server();
	 Server(std::multimap<std::string, std::string>& config);
	 ~Server();
	 Server(const Server& other);
	 Server& operator=(const Server& other);
	 void	setSocket(void);
	 void	runServer(void);
	 std::string readFd(int* client_fd);
	 void	sendRequest(Request& request, int client_fd);
	 void	sendToClient(std::string header, std::string buffer, int client_fd);
	 void	launchServer(void);
	 void	manage_epoll_wait(struct epoll_event &event);

	private:
	 std::multimap<std::string, std::string> _config;
	 int 		_port;
	 bool		_running;
	 Socket		_socket;
	 Epoll		_epoll;
	 FdManager	_fd;
	int			_nb_server;
 } ;

#endif // SERVER_HPP_
