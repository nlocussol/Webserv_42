#ifndef ERRORPAGE_HPP_
#define ERRORPAGE_HPP_

#include <string>

class ErrorPage { 
	public:
		static ErrorPage& getInstance();
		std::string getErrorPage(int);
		std::string getConfPage(std::string, int &);
		std::string bodySize(const std::string&);

	private:
	 ErrorPage();
	 ErrorPage(const ErrorPage& other);
	 ErrorPage& operator=(const ErrorPage& other);

	 static	std::string _301Header;
	 static	std::string _100Header;

	 static std::string _400Header;
	 static std::string _400Body;
	 static std::string _403Header;
	 static std::string _403Body;
	 static std::string _404Header;
	 static std::string _404Body;
	 static std::string _405Header;
	 static std::string _405Body;
	 static std::string _414Header;
	 static std::string _414Body;
	 static	std::string _500Header;
	 static	std::string _500Body;
	 static	std::string _507Header;
	 static	std::string _507Body;
	 static	std::string _508Header;
	 static	std::string _508Body;
 } ;

#endif // ERRORPAGE_HPP_
