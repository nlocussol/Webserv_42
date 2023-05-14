#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <iostream>
# include <vector>
# include "webserv.hpp"

typedef enum readStatus{
	DEL,
	FINISH,
	IN_PROGRESS
} readStatus;

class Client
{
	friend class Server;

	 public:
		Client(int fdClient, int fdServer, int idServer);
		~Client();
		Client (const Client &copy);
		Client &operator=(const Client&);
	
		int readFromFd();
		void resetBuffer();
		int		getFdClient() const;
		int		getFdServer() const;
		int		getIdServer() const;
		std::string getBuffer() const;
	 private:
		std::string _buffer;
		size_t _pos;
		int _readReturn;
		int _fdClient;
		int _fdServer;
		int _idServer;
		
};
#endif
