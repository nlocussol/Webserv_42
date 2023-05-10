#ifndef ERRORPAGE_HPP_
#define ERRORPAGE_HPP_

#include <string>

class ErrorPage { 
	public:
		static ErrorPage& getInstance();
		std::string getErrorPage(int);
		std::string getConfPage(std::string, int &);

	private:
	 ErrorPage();
	 ErrorPage(const ErrorPage& other);
	 ErrorPage& operator=(const ErrorPage& other);
	 static std::string _404Page;
	 static std::string _403Page;
	 static std::string _405Page;
	 static std::string _400Page;
	 static	std::string _507Page;
	 static	std::string _508Page;
 } ;

#endif // ERRORPAGE_HPP_
