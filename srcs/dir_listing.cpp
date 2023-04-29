#include <stdexcept>
#include <string>
#include <sys/types.h>
#include <dirent.h>
#include <libgen.h>
#include <iostream>

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

    if ((dir = opendir (path.c_str())) == NULL) {
		throw std::logic_error("Cannot open " + path);
    }
    while ((dp = readdir (dir)) != NULL) {
		body += "\t\t\t<li><a href=\"" + path + dp->d_name +  "\">" + dp->d_name + "</a></li>\r\n";
		
	}
	body += "\t\t</ul>\r\n\
	</body>\r\n\
</html>\r\n";
	closedir(dir);
	return (body);
}
