#include "../inc/Client.hpp"
#include "../inc/webserv.hpp"
#include <sys/socket.h>
#include <unistd.h>
#include <utility>
#include <iostream>

Client::Client(int fdClient, int fdServer, int idServer){
	_pos = 0;
	_fdClient = fdClient;
	_fdServer = fdServer;
	_idServer = idServer;
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
		_buffer.assign(copy._buffer, 0, copy._buffer.length());
		_pos = copy._pos;
	}
	return (*this);
}

int	Client::readFromFd()
{
	if (_pos + BUFFER_SIZE >= _buffer.max_size()){		
		std::cout << "Limit exceeded" << std::endl;
		return (-1);
	}

	_buffer.resize(_pos + BUFFER_SIZE);
	//_readReturn = recv(_fdClient, (char*)_buffer.c_str() + _pos, BUFFER_SIZE - 1, 0);
	_readReturn = read(_fdClient, (char*)_buffer.c_str() + _pos, BUFFER_SIZE - 1);
	if (_readReturn < 0) {
		std::cerr << "Error while reading from client FD" << _readReturn << endl;
		return -1;
	}
	_pos += _readReturn;
	std::cout << "size read: " << _readReturn << " lenbffer: " << _buffer.length() << " pos:" << _pos << std::endl;
	if (_readReturn >= BUFFER_SIZE - 1 || (_readReturn >= BUFFER_SIZE / 2 - 29 && _readReturn <= BUFFER_SIZE / 2 +  29))
		return (IN_PROGRESS);
	else if (_readReturn < BUFFER_SIZE && _readReturn > 0)
	{
		//_buffer.erase(_pos + 1);
		//_buffer.resize(_pos);
		//std::cout << _buffer << std::endl;
		return (FINISH);
	}
	std::cout << "default case" << std::endl;
	return (DEL);
}

void	Client::resetBuffer()
{
	_pos = 0;
	_buffer.clear();
	_buffer = "";
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
