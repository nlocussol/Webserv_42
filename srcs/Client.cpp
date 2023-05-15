#include "../inc/Client.hpp"
#include <bits/types/clock_t.h>
#include <ctime>

Client::Client(int fdClient, int idServer){
	_pos = 0;
	_readReturn = 0;
	_fdClient = fdClient;
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
		_fdClient = copy._fdClient;
		_idServer = copy._idServer;
		_buffer.assign(copy._buffer, 0, copy._buffer.length());
		_pos = copy._pos;
	}
	return (*this);
}

/**
 *
 * First of all, check if the string max size is not exceeded
 *
 * increase the buffer of the buffer size
 * usleep for the client tot send the entire request
 * read this request
 * check if the size read is egal as buffer size
 * 		need to call recv again
 * if size read is less than buffer size
 * 		reading the request is done
 * the del return is when recv return 0
 *
 **/
int	Client::readFromFd()
{
	if (_pos + BUFFER_SIZE >= _buffer.max_size()){		
		std::cout << "Limit exceeded" << std::endl;
		return (-1);
	}

	_buffer.resize(_pos + BUFFER_SIZE);
	usleep(500);
	// clock_t current = clock();
	// for (clock_t begin = clock(); current - begin / CLOCKS_PER_SEC * 1000000 < 500; current = clock())
	// {}
	_readReturn = recv(_fdClient, (char*)_buffer.c_str() + _pos, BUFFER_SIZE - 1, 0);
	if (_readReturn < 0) {
		std::cerr << "Error while reading from client FD" << _readReturn << endl;
		return -1;
	}
	_pos += _readReturn;
	if (_readReturn >= BUFFER_SIZE - 1 || (_readReturn >= BUFFER_SIZE / 2 - 29 && _readReturn <= BUFFER_SIZE / 2 +  29))
		return (IN_PROGRESS);
	else if (_readReturn < BUFFER_SIZE && _readReturn > 0)
		return (FINISH);
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

int	Client::getIdServer() const{
	return(_idServer);
}
