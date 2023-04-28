#ifndef REQUEST_HPP_
#define REQUEST_HPP_

#include <string>

class Request { 
	public:
	 Request();
	 ~Request();
	 void parseRequest();
	 void findRequestType();
 	 void findRequestSubType();
	 int getRequestType(void) const; 
	 int getRequestSubType(void) const; 
	 void setBuffer(const std::string&);

	private:
	 int _requestType;
	 int _requestSubType;
	 std::string _buffer;
 } ;

#endif // REQUEST_HPP_
