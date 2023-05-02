#include <stdexcept>
#include <string>
#include <sys/types.h>
#include <dirent.h>
#include <libgen.h>
#include <iostream>
#include "../inc/parsing.hpp"

MULTIMAP find_location_path(const string &path, block_serv servers) {
	for (unsigned long i = 0; i < servers.v_location.size(); i++) {
		if (servers.v_location[i].path == path)
			return (servers.v_location[i].conf_location);
	}
	return (servers.conf_serv);
}

bool	is_dir_listing(std::string path, block_serv & servers)
{
	DIR *dir;
  if ((dir = opendir (path.c_str())) == NULL)
  	return false;
  closedir(dir);

	MULTIMAP copy = find_location_path(path, servers);
	MULTIMAP::iterator it = copy.find("autoindex");
	if (it != copy.end()
		&& it->second == "on" && copy.find("index") == copy.end())
		return true;
	return false;
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

std::string directory_listing(std::string path)
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
    while ((dp = readdir (dir)) != NULL) {
			std::string test = dp->d_name;
			body += "\t\t\t<li><a href=\"" + path + "/" + test +  "\">" + dp->d_name + "</a></li>\r\n";
		}
	body += "\t\t</ul>\r\n\
	</body>\r\n\
</html>\r\n";
	closedir(dir);
	return (body);
}
