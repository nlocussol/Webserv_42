#include <stdexcept>
#include <string>
#include <sys/types.h>
#include <dirent.h>
#include <libgen.h>
#include <iostream>
#include "../inc/parsing.hpp"
#include "../inc/webserv.hpp"

MULTIMAP find_location_path(string &path, block_serv servers) {
	string str;
	if (!is_dir(path))
		str = path.substr(0, path.find_last_of('/'));
	else
		str = path;
	MULTIMAP::iterator it;
	for (unsigned long i = 0; i < servers.v_location.size(); i++) {
		it = servers.v_location[i].conf_location.find("root");
		string loc_path = it->second;
		if (loc_path[loc_path.size() - 1] == '/' && servers.v_location[i].path[0] == '/')
			loc_path += servers.v_location[i].path.substr(1);
		else
			loc_path += servers.v_location[i].path;
		if (str[str.size() - 1] != '/')
			str += "/";
		if (str == loc_path) {
			return (servers.v_location[i].conf_location);
		}
	}
	return (servers.conf_serv);
}

bool is_dir(std::string& path)
{
	DIR *dir;
  if ((dir = opendir (path.c_str())) == NULL)
  	return false;
  closedir(dir);
  return true;
}

int	is_dir_listing(std::string path, block_serv & servers)
{
	DIR *dir;
  if ((dir = opendir (path.c_str())) == NULL)
  	return NOT_DIR;
  closedir(dir);

	MULTIMAP copy = find_location_path(path, servers);
	MULTIMAP::iterator it = copy.find("autoindex");
	if (it != copy.end()
		&& it->second == "on" && copy.find("index") == copy.end())
		return AUTOINDEX_OK;
	return AUTOINDEX_OFF;
}

/**
 *
 * Parameter : a path to the directory to list
 *
 * Func :
 *  Create a simple html page inside a std::string
 * 	open the directory given as parameter
 * 	add html link to each file inside the open directory
 * 	with the <a href="path">name</a> syntax
 * 	and complete the html
 * 	return the html page stored inside std::string
 *
**/

std::string directory_listing(std::string path, block_serv& server)
{
    DIR *dir;
    struct dirent *dp;
    std::string body;

	body += "<!DOCTYPE html>\r\n\
<html>\r\n\
	<head>\r\n\
		<title>Directory listing</title>\r\n\
		<meta http-equiv=\"content-type\" content=\"text/html;charset=utf-8\">\r\n\
	</head>\r\n\
	<body>\r\n\
		<ul>\r\n";

    dir = opendir (path.c_str()); 
	std::string ls;
	MULTIMAP map = find_location_path(path, server);
	std::string root = map.find("root")->second;
	std::string root2 = root.erase(root.length() - 1);
	path.erase(0, root.size());
    while ((dp = readdir (dir)) != NULL) {
		ls = dp->d_name;
		if (ls == "." || ls == "..")
			continue ;
		body += "\t\t\t<li><a href=\"" + path;
		if (path[path.size() - 1] != '/')
			body += "/";
		body += ls + "\">" + ls + "</a></li>\r\n";
	}
	body += "\t\t</ul>\r\n\
	</body>\r\n\
</html>\r\n";
	closedir(dir);
	return (body);
}
