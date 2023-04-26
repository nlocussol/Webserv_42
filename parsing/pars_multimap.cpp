#include "parsing.hpp"
#include <dirent.h>

void pars_dir(string path, MULTIMAP & copy) {
	MULTIMAP::iterator it = copy.find(path);
	if (it == copy.end() && path == "root")
		throw (logic_error("Error: need a root in servers block!"));
	while (it != copy.end()) {
		DIR *file = opendir(it->second.c_str());
		if (!file)
			throw (logic_error("Error: " + it->first + " have a bad path: " + it->second));
		closedir(file);
		copy.erase(it);
		it = copy.find(path);
	}
}

void pars_file(string path, MULTIMAP & copy, string & root) {
	MULTIMAP::iterator it = copy.find(path);
	while (it != copy.end()) {
		fstream file;
		file.open((root + "/" + it->second).c_str());
		if (!file)
			throw (logic_error("Error: " + it->first + " have a bad path: " + it->second));
		copy.erase(it);
		it = copy.find(path);
	}
}

void pars_listen(MULTIMAP & copy) {
	MULTIMAP::iterator it = copy.find("listen");
	if (it == copy.end())
		throw (logic_error("Error: need listen in servers block!"));
	while (it != copy.end()) {
		for (int i = 0; it->second[i]; i++) {
			if (!isdigit(it->second[i]))
				throw (logic_error("Error: listen has bad input: " + it->second));
		}
		int port = strtod(it->second.c_str(), NULL);
		if (port < 0 || port > 65535)
				throw (logic_error("Error: listen have bad port: " + it->second + " (between 0 and 65535)!"));
		copy.erase(it);
		it = copy.find("listen");
	}
}

void pars_methods(MULTIMAP & copy) {
	MULTIMAP::iterator it = copy.find("methods");
	while (it != copy.end()) {
		if (it->second != "GET" && it->second != "POST" && it->second != "DELETE")
			throw (logic_error("Error: bad methods: " + it->second + " (only GET, POST and DELETE)!"));
		copy.erase(it);
		it = copy.find("methods");
	}
}

void pars_errpage(MULTIMAP & copy, string & root) {
	MULTIMAP::iterator it = copy.find("errpage");
	fstream file; 
	while (it != copy.end()) {
		for (int i = 0; it->second[i]; i++) {
			if (!isdigit(it->second[i]))
				throw (logic_error("Error: errpage has bad input: " + it->second + " (between 0 and 599)!"));
		}
		copy.erase(it);
		it = copy.find("errpage");
		file.open((root + "/" + it->second).c_str());
		if (!file)
			throw (logic_error("Error: errpage redirection can't be open: " + it->second));
		copy.erase(it);
		it = copy.find("errpage");
	}
}
