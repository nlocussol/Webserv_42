#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <iostream>

class Client
{
	 public:
		Client(int fdClient, int fdServer, int idServer);
		~Client();
		Client (const Client &copy);
		Client &operator=(const Client&);
		
		int		getFdClient() const;
		int		getFdServer() const;
		int		getIdServer() const;
		std::string	getBuffer() const;
		void	setBuffer(std::string);
	 private:
		std::string	_buffer;
		int			_fdClient;
		int			_fdServer;
		int			_idServer;
		
};
#endif
