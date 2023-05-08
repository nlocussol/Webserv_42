#ifndef webserv
#define webserv

#include <iostream>
#include "parsing.hpp"
#include <cstddef>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#define TIME_OUT 1
#define PERM_DENIED 2
#define MAX_CGI_WAITING 3
#define RUNTIME_ERROR 4

enum request_type {UNSUPPORTED_REQUEST = 1, GET_REQUEST, POST_REQUEST, DELETE_REQUEST, DIR_LISTING, ERROR_RESPONSE};
enum request_sub_type {TEXT = 1, IMAGE, VIDEO, QUERY};
enum autoindex {NOT_DIR, AUTOINDEX_OK, AUTOINDEX_OFF};

int		handle_cgi(block_serv server, std::string exec);
bool	is_cgi(block_serv server, std::string file);
MULTIMAP find_location_path(const string &path, block_serv servers);
int	is_dir_listing(std::string path, block_serv & servers);
std::string directory_listing(std::string path, block_serv &server);

#endif
