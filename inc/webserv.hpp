#ifndef webserv
#define webserv

#include <iostream>
#include "parsing.hpp"
#include "Request.hpp"
#include <cstddef>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#define TIME_OUT 1
#define PERM_DENIED 2
#define MAX_CGI_WAITING 3
#define RUNTIME_ERROR 4

#define BUFFER_SIZE 262144

enum request_type {UNSUPPORTED_REQUEST = 1, GET_REQUEST, POST_REQUEST, DELETE_REQUEST, DIR_LISTING, ERROR_RESPONSE};
enum request_sub_type {DEFAULT, TEXT, IMAGE, VIDEO, QUERY};
enum autoindex {NOT_DIR, AUTOINDEX_OK, AUTOINDEX_OFF};

string handle_cgi(block_serv server, std::string exec, int *flag, Request&);
bool	is_cgi(block_serv server, std::string file);
MULTIMAP find_location_path(string &path, block_serv servers);
int	is_dir_listing(std::string path, block_serv & servers);
bool is_dir(std::string&);
std::string directory_listing(std::string path, block_serv &server);
std::string itostr(int i);
char** vector_to_c_array(const std::vector<std::string>& queryArg);
std::vector<std::string> string_to_vector(std::string& bodyContent);
int checkHexa(std::string& line, std::string hexa);

#endif
