#ifndef REQUEST_HPP_
#define REQUEST_HPP_

#include <string>

class Request { 
	public:
	 typedef enum request_type {TEXT_HTML = 1} t_request;
	 void buildTextHTMLRequest(std::string);
	 std::string& getBuffer(void);
	 Request(int);
	 ~Request();

	private:
	 Request();
	 Request(const Request& other);
	 Request& operator=(const Request& other);

	private:
	 std::string _buffer;
 } ;

#endif // REQUEST_HPP_
