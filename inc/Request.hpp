#ifndef REQUEST_HPP_
#define REQUEST_HPP_

#include <string>

#define BUFFER_SIZE 1024

class Request { 
	public:
	 void buildGetResponse(std::string);
	 std::string& getBuffer(void);
	 Request(int, int, int, std::string);
	 ~Request();

	private:
	 Request();
	 Request(const Request& other);
	 Request& operator=(const Request& other);
	 void handleError(int);
	 typedef enum request_type {UNSUPPORTED_REQUEST, GET_REQUEST, POST_REQUEST, DELETE_REQUEST} e_request;
	 typedef enum request_sub_type {TEXT, IMAGE} t_request;

	private:
	 std::string _buffer;

	 static std::string _404;
 } ;

#endif // REQUEST_HPP_
