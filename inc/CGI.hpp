#ifndef CGI_HPP_
# define CGI_HPP_

# include "webserv.hpp"

class CGI { 
	public:
	 CGI(std::string&, std::string&, Request);
	 ~CGI();
	 int handleCGI(Request& request);
	 int check_cgi_args();
	 bool check_time();
	 std::string get_output_cgi();
	 int getFlag() const;
	 void setClientFd(int clientFd);

	private:
	 int _pipeIn[2];
	 int _pipeOut[2];
	 int _pid;
	 int _flag;
	 int _clientFd;
	 std::string _binCGI;
	 std::string _filePath;
	 std::string _cookies;
	 std::string _queryString;
	 std::string _postBody;
	 std::vector<std::string> _vectorEnv;
 } ;

std::string	is_cgi(block_serv, const std::string& filePath);

#endif // CGI_HPP_
