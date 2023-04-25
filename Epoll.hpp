#include <sys/epoll.h>
#include "iostream"

class Epoll
{
	 public:
		Epoll();
		~Epoll();
		Epoll (const Epoll &copy);
		Epoll &operator=(const Epoll&);

		void	add_fd_to_pool(int fd);
		void	create_epoll();
		int		get_fd_epoll();
	 private:

		int			_fd_epoll;
		struct epoll_event	_event;
		
};
