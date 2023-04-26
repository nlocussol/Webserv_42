#ifndef REQUEST_HPP_
#define REQUEST_HPP_

#include <string>

#define BUFFER_SIZE 1024

class Request { 
	public:
	 typedef enum request_type {TEXT_HTML = 1, IMAGE} t_request;
	 void buildTextHTMLRequest(std::string);
	 std::string& getBuffer(void);
	 Request(int,int, std::string);
	 ~Request();

	private:
	 Request();
	 Request(const Request& other);
	 Request& operator=(const Request& other);
	 void handleError(int);

	private:
	 std::string _buffer;

	 static std::string _404;
 } ;

#endif // REQUEST_HPP_
