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
#include "FdManager.hpp"
#include "parsing.hpp"

#define MAX_EVENT 10

class Server { 
	public:
	 Server(data);
	 ~Server();
	 Server(const Server&);
	 Server& operator=(const Server&);
	 void	setSocket(void);
	 void	runServer(void);
	 void readRequest(int epoll_fd);
	 int handlePostRequest(int);
	 bool isFileProtected() const;
	 int handleDeleteRequest(int);
	 void	sendResponse(Response&, int);
	 void	manage_epoll_wait(struct epoll_event&);
	 static bool _running;

	private:
	 Socket _socket;
	 Epoll _epoll;
	 FdManager _fd;
	 int _nb_server;
	 std::string _buffer;
	 data _servers;
 } ;

void	handle_sigint(int signum);

#endif // SERVER_HPP_
