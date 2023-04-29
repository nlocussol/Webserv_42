#include <stdexcept>
#include <string>
#include <sys/types.h>
#include <dirent.h>
#include <libgen.h>
#include <iostream>

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
		throw std::logic_error("Cannot open" + path);
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
