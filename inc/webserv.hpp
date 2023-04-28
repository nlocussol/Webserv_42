#ifndef webserv
#define webserv

#include <iostream>
#include "parsing.hpp"
#include <cstddef>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#define BUFFER_SIZE 2048

int handle_cgi(std::string inter, std::string exec);
bool is_cgi(MULTIMAP map, std::string file);

typedef enum request_type {UNSUPPORTED_REQUEST, GET_REQUEST, POST_REQUEST, DELETE_REQUEST} e_request;
typedef enum request_sub_type {TEXT, IMAGE, VIDEO} e_requestsubtype;

#endif
