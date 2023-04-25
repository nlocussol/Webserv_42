#ifndef REQUEST_HPP_
#define REQUEST_HPP_

#include <string>

#define BUFFER_SIZE 1024

class Request { 
	public:
	 typedef enum request_type {TEXT_HTML, IMAGE} t_request;
	 void buildTextHTMLRequest(std::string);
	 std::string& getBuffer(void);
	 Request(int, std::string);
	 ~Request();

	private:
	 Request();
	 Request(const Request& other);
	 Request& operator=(const Request& other);

	private:
	 std::string _buffer;
 } ;

#endif // REQUEST_HPP_
