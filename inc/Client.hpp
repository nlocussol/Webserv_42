#ifndef CLIENT_HPP
# define CLIENT_HPP
# include <vector>
# include <map>
# include <algorithm>
# include <stdexcept>
# include <unistd.h>

class Client
{
	 public:
		Client(int fdClient, int fdServer, int idServer);
		~Client();
		Client (const Client &copy);
		Client &operator=(const Client&);
		
		int	getFdClient() const;
		int	getFdServer() const;
		int	getIdServer() const;
	 private:
		std::string	_buffer;
		int	_fdClient;
		int	_fdServer;
		int	_idServer;
		
};
#endif
