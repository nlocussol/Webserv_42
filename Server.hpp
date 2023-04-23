#ifndef SERVER_HPP_
#define SERVER_HPP_

#include <map>
#include <netinet/in.h>
#include <string>

class Server { 
	public:
	 Server();
	 Server(std::multimap<std::string, std::string>& config);
	 ~Server();
	 Server(const Server& other);
	 Server& operator=(const Server& other);
	 void setSocket(void);
	 void runServer(void);
	 void sendToClient(std::string header, std::string buffer, int client_fd);
	 void launchServer(void);

	private:
	 std::multimap<std::string, std::string> _config;
	 int _socket;
	 int _port;
	 bool _running;
	 struct sockaddr_in _serv_addr;
 } ;

#endif // SERVER_HPP_
