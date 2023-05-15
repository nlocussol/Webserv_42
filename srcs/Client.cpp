#include "../inc/Client.hpp"

Client::Client(int fdClient, int idServer){
	_pos = 0;
	_readReturn = 0;
	_fdClient = fdClient;
	_idServer = idServer;
	_cgiFd = -1;
	_cgiOver = false;
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
		_cgiFd = copy._cgiFd;
		_cgiOver = copy._cgiOver;
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
		std::cerr << "Limit exceeded" << std::endl;
		return (-1);
	}

	_buffer.resize(_pos + BUFFER_SIZE);
	clock_t begin = clock();
	clock_t end = begin +  (500 * CLOCKS_PER_SEC) / 1000000;
	while (clock() < end)
	{}
	if (_cgiFd != -1)
		_readReturn = read(_cgiFd, (char*)_buffer.c_str() + _pos, BUFFER_SIZE - 1);
	else
		_readReturn = recv(_fdClient, (char*)_buffer.c_str() + _pos, BUFFER_SIZE - 1, 0);
	if (_readReturn < 0) {
		return -1;
	}
	_pos += _readReturn;
	if (_readReturn >= BUFFER_SIZE - 1 || (_readReturn >= BUFFER_SIZE / 2 - 29 && _readReturn <= BUFFER_SIZE / 2 +  29))
		return (IN_PROGRESS);
	else if (_readReturn < BUFFER_SIZE && _readReturn > 0)
	{
		if (_cgiFd != -1)
			_cgiOver = true;
		return (FINISH);
	}
	return (DEL);
}

void	Client::resetBuffer()
{
	_pos = 0;
	_buffer.clear();
	_buffer = "";
	if (_cgiFd != -1)
		close (_cgiFd);
	_cgiFd = -1;

}
int	Client::getFdClient() const{
	return(_fdClient);
}

int	Client::getIdServer() const{
	return(_idServer);
}

int	Client::getCgiFd() const{
	return(_cgiFd);
}

void	Client::setCgiFd(int cgiFd){ _cgiFd = cgiFd; }
