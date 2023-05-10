#include "../inc/Client.hpp"
#include "../inc/webserv.hpp"
#include <sys/socket.h>
#include <unistd.h>
#include <utility>
#include <iostream>

Client::Client(int fdClient, int fdServer, int idServer){
	_fdClient = fdClient;
	_fdServer = fdServer;
	_idServer = idServer;
	_pos = 0;
	_readReturn = 0;
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

void Client::readFromFd()
{
	_buffer.resize(_pos + BUFFER_SIZE);
	_readReturn = recv(_fdClient, (char*)_buffer.c_str() + _pos, BUFFER_SIZE - 1, MSG_DONTWAIT);
	if (_readReturn < 0) {
		std::cerr << "Error while reading from client FD\n";
		return ;
	}
	_pos += _readReturn;
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

std::string Client::getBuffer() const{
	return(_buffer);
}

void	Client::setBuffer(std::string newBuffer) {
	_buffer = newBuffer;
}
