#ifndef SERVER_HPP_
#define SERVER_HPP_

#include <map>
#include <netinet/in.h>
#include <string>
#include <sys/epoll.h>
#include <cstring>
#include <signal.h>
#include "Request.hpp"
#include "Epoll.hpp"
#include "Socket.hpp"
#include "FdManager.hpp"
#include "parsing.hpp"

#define MAX_EVENT 10

class Server { 
	public:
	 Server(data servers);
	 Server(std::multimap<std::string, std::string>& config);
	 ~Server();
	 Server(const Server& other);
	 Server& operator=(const Server& other);
	 void	setSocket(void);
	 void	runServer(void);
	 void readRequest(int epoll_fd);
	 int findRequestType();
	 int findRequestSubType();
	 int handleGetRequest(int);
	 void	sendRequest(Request& request, int client_fd);
	 void	manage_epoll_wait(struct epoll_event &event);
	 static bool _running;

	private:
	 std::multimap<std::string, std::string> _config;
	 Socket _socket;
	 Epoll _epoll;
	 FdManager _fd;
	 int _nb_server;
	 std::string _buffer;
	 std::string _filePath;
	 int _statusCode;
	 data _servers;
	 typedef enum request_type {UNSUPPORTED_REQUEST, GET_REQUEST, POST_REQUEST, DELETE_REQUEST} e_request;
	 typedef enum request_sub_type {TEXT, IMAGE} t_request;
 } ;

void	handle_sigint(int signum);

#endif // SERVER_HPP_
