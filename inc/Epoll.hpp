#ifndef EPOLL_HPP
# define EPOLL_HPP

# include <sys/epoll.h>
# include <iostream>
# include <cstring>
# include <cstdlib>
# include <unistd.h>

class Epoll
{
	 public:
		Epoll();
		~Epoll();
		Epoll (const Epoll &copy);
		Epoll &operator=(const Epoll&);

		void	add_fd_to_pool(int fd);
		void	del_fd_from_pool(int fd);
		void	create_epoll();
		int		get_fd_epoll();

	 private:
		int			_fd_epoll;
		struct epoll_event	_event;
};

#endif
