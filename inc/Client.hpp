#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "webserv.hpp"
# include <sys/socket.h>

typedef enum readStatus{
	DEL,
	FINISH,
	IN_PROGRESS
} readStatus;

class Client
{
	friend class Server;

	 public:
		Client(int fdClient, int idServer);
		~Client();
		Client (const Client &copy);
		Client &operator=(const Client&);
	
		int readFromFd();
		void resetBuffer();
		int		getFdClient() const;
		int		getIdServer() const;
		int		getCgiFd() const;
		void	setCgiFd(int);
	 private:
		std::string _buffer;
		size_t _pos;
		int _readReturn;
		int _fdClient;
		int _idServer;
		int	_cgiFd;
	 	bool _cgiOver;
};
#endif
