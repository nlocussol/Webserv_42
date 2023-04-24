#include <sys/epoll.h>
#include "iostream"

class Epoll
{
	 public:
		Epoll();
		~Epoll();
		Epoll (const Epoll &copy);
		Epoll &operator=(const Epoll&);

		void	set_up_epoll(int socket_server);
		void	add_fd_to_pool(int fd);
	 private:
		void	create_epoll();

		int _fd_epoll;
		struct epoll_event _event;
		
};
