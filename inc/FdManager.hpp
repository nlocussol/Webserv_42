#ifndef FDMANAGER_HPP
# define FDMANAGER_HPP
# include <vector>
# include <map>
# include <algorithm>
# include <stdexcept>
# include <unistd.h>

class FdManager
{
	 public:
		FdManager();
		~FdManager();
		FdManager (const FdManager &copy);
		FdManager &operator=(const FdManager&);

		void	set_fd_servers(int server_socket, int server_id);
		void	add_new_client(int new_client, int server);
		bool	is_server(int socket);
		int		find_matching_server(int client);
		void	close_fd(int client);
		void	close_every_fd();
	 private:
		//first-> fd of the server / second -> his index based on conf file
		std::map<int, int>	_fd_servers;
		//first-> fd of the client / second -> his server fd
		std::map<int, int>	_fd_pool;
		
};
#endif
