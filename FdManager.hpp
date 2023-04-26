#include <vector>
#include <map>
#include <algorithm>
#include <stdexcept>

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
	 private:
		std::map<int, int>	_fd_servers;
		std::map<int, int>	_fd_pool;
		
};
