#ifndef REQUEST_HPP_
#define REQUEST_HPP_

#include <string>

class Request { 
	public:
	 Request();
	 ~Request();
	 void findRequestType(const std::string&);
 	 void findRequestSubType(const std::string& buffer);
	 int getRequestType(void) const; 
	 int getRequestSubType(void) const; 

	private:
	 int _requestType;
	 int _requestSubType;
	 Request(const Request& other);
	 Request& operator=(const Request& other);
 } ;

#endif // REQUEST_HPP_
