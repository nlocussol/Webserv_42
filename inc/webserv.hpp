#ifndef webserv
#define webserv

#include <iostream>
#include "parsing.hpp"
#include <cstddef>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

int	handle_cgi(std::string inter, std::string exec);
bool	is_cgi(MULTIMAP map, std::string file);

#endif
