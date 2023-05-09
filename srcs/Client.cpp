#include "../inc/Client.hpp"
#include <unistd.h>
#include <utility>
#include <iostream>

Client::Client(int fdClient, int fdServer, int idServer){
	_fdClient = fdClient;
	_fdServer = fdServer;
	_idServer = idServer;
}

Client::~Client(){
}

Client::Client(const Client &copy){
	*this = copy;
}

Client & Client::operator=(const Client &copy){
	if (&copy != this)
	{
		_fdClient = copy.getFdClient();
		_fdServer = copy.getFdServer();
		_idServer = copy.getIdServer();
		_buffer = copy._buffer;
	}
	return (*this);
}

int	Client::getFdClient() const{
	return(_fdClient);
}

int	Client::getFdServer() const{
	return(_fdServer);
}

int	Client::getIdServer() const{
	return(_idServer);
}
