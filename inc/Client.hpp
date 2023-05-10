#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <iostream>

class Client
{
	friend class Server;

	 public:
		Client(int fdClient, int fdServer, int idServer);
		~Client();
		Client (const Client &copy);
		Client &operator=(const Client&);
	
		void readFromFd();
		int		getFdClient() const;
		int		getFdServer() const;
		int		getIdServer() const;
		std::string	getBuffer() const;
		void	setBuffer(std::string);
	 private:
		std::string	_buffer;
		size_t _pos;
		int _readReturn;
		int			_fdClient;
		int			_fdServer;
		int			_idServer;
		
};
#endif
