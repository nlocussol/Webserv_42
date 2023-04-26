#include "../inc/FdManager.hpp"
#include <utility>
#include <iostream>

FdManager::FdManager(){
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

void	FdManager::set_fd_servers(int server_socket, int server_id)
{
	std::map<int, int>::iterator check;
	check  = _fd_servers.find(server_socket);
	if (check != _fd_servers.end()){
		throw std::logic_error("The socket is already inside the server map");
	} else {
		_fd_servers.insert(std::make_pair(server_socket, server_id));
	}
}

void	FdManager::add_new_client(int new_client, int server)
{
	std::map<int, int>::iterator check;

	check  = _fd_pool.find(server);
	std::cout << "client : " << new_client << std::endl;
	std::cout << "server : " << server << std::endl;
	std::cout << "check : " << check->second << std::endl;
	if (check != _fd_pool.end()){
		throw std::logic_error("The server it's set");
	} else {
		_fd_pool.insert(std::make_pair(new_client, server));
	}
}

bool	FdManager::is_server(int socket)
{
	std::map<int, int>::iterator check;
	check  = _fd_servers.find(socket);
	if (check == _fd_servers.end()){
		return (false);
	} else {
		return (true);
	}
}


int		FdManager::find_matching_server(int client)
{
	std::map<int, int>::iterator check;
	check = _fd_pool.find(client);
	if (check == _fd_pool.end()){
		throw std::logic_error("Can't find the client inside the pool");
	} else {
		std::cout << "client : " << client << " server : " << check->second << std::endl;
		return (check->second);
	}
}

