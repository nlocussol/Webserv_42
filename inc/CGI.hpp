#ifndef CGI_HPP_
#define CGI_HPP_

#include <string>
#include "Request.hpp"

class CGI { 
	public:
	 CGI(std::string&, std::string&);
	 ~CGI();
	 std::string handleCGI(const Request& request);
	 int check_cgi_args();
	 bool check_time();
	 std::string get_output_cgi();
	 int getFlag() const;

	private:
	 int _pip[2];
	 int _pid;
	 int _flag;
	 std::string _binCGI;
	 std::string _filePath;
	 std::vector<std::string> _cookies;
	 std::string _queryStr;
	 std::string _postBody;
	 std::vector<std::string> _vEnc;
 } ;

std::string	is_cgi(block_serv, const std::string& filePath);

#endif // CGI_HPP_
