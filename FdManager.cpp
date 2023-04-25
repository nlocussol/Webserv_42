#include "FdManager.hpp"
#include <utility>

FdManager::FdManager(){
}

FdManager::FdManager(int nb_server){
}

FdManager::~FdManager(){
}

FdManager::FdManager(const FdManager &copy){
	*this = copy;
}

FdManager & FdManager::operator=(const FdManager &copy){
	if (&copy != this)
	{

	}
	return (*this);
}

void	FdManager::set_fd_server(int server_socket)
{
	std::vector<int>::iterator check;
	check  = std::find(_fd_server.begin(), _fd_server.end(), server_socket);
	if (check != _fd_server.end()){
		throw std::logic_error("The socket is already inside the vector");
	} else {
		_fd_server.push_back(server_socket);
	}
}

void	FdManager::add_new_client(int new_client, int server)
{
	_fd_map.insert(std::make_pair(new_client, server));
}

bool	FdManager::is_server(int socket)
{
	std::vector<int>::iterator check;
	check  = std::find(_fd_server.begin(), _fd_server.end(), socket);
	if (check == _fd_server.end()){
		return (false);
	} else {
		return (true);
	}
}


int		FdManager::find_matching_server(int client)
{
	std::map<int, int>::iterator check;
	check = _fd_map.find(client);
	if (check == _fd_map.end()){
		throw std::logic_error("Can't find the client inside the pool");
	} else {
		return (check->second);
	}
}

