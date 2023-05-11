#include "../../inc/parsing.hpp"
#include "../../inc/webserv.hpp"
#include <sstream>

void pars_rewrite(MULTIMAP & copy) {
	MULTIMAP::iterator it = copy.find("rewrite");
	MULTIMAP::iterator root = copy.find("root");
	while (it != copy.end()) {
		string dir = it->second;
		if (dir[0] == '/')
			dir = it->second.substr(1);
		dir = root->second + dir;
		if (!is_dir(dir))
			throw (logic_error("Error: rewrite argument can be only a directory: " + it->second));
		copy.erase(it);
		it = copy.find("rewrite");
	}
}

void pars_body_size(MULTIMAP & copy) {
	MULTIMAP::iterator it = copy.find("limit_client_body_size");
	while (it != copy.end()) {
		for (int i = 0; it->second[i]; i++) {
			if (!isdigit(it->second[i]))
				throw (logic_error("Error: body size take a number as parameter: " + it->second));
		}
		double limit = strtod(it->second.c_str(), NULL);
		if (limit < 0)
			throw (logic_error("Error: body size can't be negative: " + it->second ));
		copy.erase(it);
		it = copy.find("limit_client_body_size");
	}
}

void pars_dir(string path, MULTIMAP & copy) {
	MULTIMAP::iterator it = copy.find(path);
	if (it == copy.end() && path == "root")
		throw (logic_error("Error: need a root in servers block!"));
	while (it != copy.end()) {
		DIR *file = opendir(it->second.c_str());
		if (!file)
			throw (logic_error("Error: " + it->first + " have a bad path: " + it->second));
		if (path == "root") {
			string root = it->second.substr(0, it->second.find('/') + 1);
			if (root == "srcs/" || root == "inc/" || root == "conf/")
				throw (logic_error("Error: " + it->first + " can't start with: " + root));
		}
		closedir(file);
		copy.erase(it);
		it = copy.find(path);
	}
}

void pars_file(string path, MULTIMAP & copy, string & root) {
	MULTIMAP::iterator it = copy.find(path);
	while (it != copy.end()) {
		fstream file;
		file.open((root + it->second).c_str());
		if (!file)
			throw (logic_error("Error: " + it->first + " have a bad path: " + it->second));
		copy.erase(it);
		it = copy.find(path);
	}
}

void pars_listen(MULTIMAP & copy, vector<int> & ports) {
	MULTIMAP::iterator it = copy.find("listen");
	if (it == copy.end())
		throw (logic_error("Error: need listen in servers block!"));
	while (it != copy.end()) {
		for (int i = 0; it->second[i]; i++) {
			if (!isdigit(it->second[i]))
				throw (logic_error("Error: listen has bad input: " + it->second));
		}
		int port = strtod(it->second.c_str(), NULL);
		ports.push_back(port);
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

void pars_errpage(MULTIMAP & copy, MULTIMAP & current, string & root) {
	MULTIMAP::iterator it = copy.find("errpage");
	fstream file; 
	while (it != copy.end()) {
		for (int i = 0; it->second[i]; i++) {
			if (!isdigit(it->second[i]))
				throw (logic_error("Error: errpage has bad input: " + it->second + " (between 100 and 599)!"));
		}
		int nb = strtod(it->second.c_str(), NULL);
		if (nb < 100 || nb > 599)
			throw (logic_error("Error: errpage has bad input: " + it->second + " (between 100 and 599)!"));
		copy.erase(it);
		it = copy.find("errpage");
		file.open((root + it->second).c_str());
		if (!file)
			throw (logic_error("Error: errpage redirection can't be open: " + it->second));
		string file = it->second;
		stringstream str;
		str << nb;
		copy.erase(it);
		current.insert(make_pair("errpage_" + str.str(), file));
		it = copy.find("errpage");
	}
}
