#include <vector>
#include <map>
#include <algorithm>
#include <stdexcept>

class FdManager
{
	 public:
		FdManager();
		FdManager(int nb_server);
		~FdManager();
		FdManager (const FdManager &copy);
		FdManager &operator=(const FdManager&);

		void	set_fd_server(int server_socket);
		void	add_new_client(int new_client, int server);
		bool	is_server(int socket);
		int		find_matching_server(int client);
	 private:
		std::vector<int>	_fd_server;
		std::map<int, int>	_fd_map;
		
};
