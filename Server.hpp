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
	 void readRequest(int epoll_fd);
	 int parseRequestType();
	 int handleGetRequest();
	 void	sendRequest(Request& request, int client_fd);
	 void	manage_epoll_wait(struct epoll_event &event);

	private:
	 std::multimap<std::string, std::string> _config;
	 int _port;
	 bool _running;
	 Socket _socket;
	 Epoll _epoll;
	 FdManager _fd;
	 int _nb_server;
	 std::string _buffer;
	 std::string _filePath;
	 int _responseCode;

	private:
	 typedef enum request_type {UNSUPPORTED_REQUEST, GET_REQUEST, POST_REQUEST, DELETE_REQUEST} e_request;
 } ;

#endif // SERVER_HPP_
