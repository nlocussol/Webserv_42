#include "../../inc/parsing.hpp"
#include "../../inc/webserv.hpp"
#include <sstream>
#include <stdexcept>
#include <string>

/* Put a slash after every root options if they don't have one */
void fill_root(MULTIMAP & current) {
	MULTIMAP::iterator it = current.find("root");
	if (it->second[it->second.size() - 1] != '/')
		it->second += "/";
}

/* Pars configuration file location block options one by one */
void pars_manager(block_location & servers, vector<int> & ports, string & path, string & old_root) {
	MULTIMAP::iterator it = servers.conf_location.find("root");
	MULTIMAP copy = servers.conf_location;

	pars_dir("root", copy);
	fill_root(servers.conf_location);
	string new_path = old_root + path;
	if (!is_dir(new_path))
		throw (logic_error("Error: location block has a bad path: " + new_path));
	pars_file("index", copy, it->second);
	pars_listen(copy, ports);
	pars_methods(copy);
	pars_errpage(copy, servers.conf_location, it->second);
	pars_body_size(copy);
	pars_rewrite(copy, it->second);
	pars_cgi(copy);
	pars_autoindex(copy);
}

/* Pars configuration file server block options one by one */
string pars_manager(block_serv & servers, vector<int> & ports) {
	MULTIMAP::iterator it = servers.conf_serv.find("root");
	MULTIMAP copy = servers.conf_serv;

	pars_dir("root", copy);
	fill_root(servers.conf_serv);
	pars_cgi(copy);
	pars_file("index", copy, it->second);
	pars_listen(copy, ports);
	pars_methods(copy);
	pars_errpage(copy, servers.conf_serv, it->second);
	pars_body_size(copy);
	pars_autoindex(copy);
	return (it->second);
}

/* Check if the server listen on the same port thann another*/
template<typename T>
void check_double(T & tab) {
	for (size_t i = 0; i < tab.size(); i++) {
		if (std::count(tab.begin(), tab.end(), tab[i]) > 1) {
			stringstream ss;
			ss << tab[i];
			throw (logic_error("Error: two serveurs can't listen on the same port: " + ss.str()));
		}
	}
}

void pars_struct(data & servers) {
	vector<int> ports;
	string root;
	for (unsigned long i = 0; i < servers.v_serv.size(); i++) {
		root = pars_manager(servers.v_serv[i], ports);
		for (unsigned long j = 0; j < servers.v_serv[i].v_location.size(); j++) {
			vector<int> tmp;
			int stage = servers.v_serv[i].v_location[j].stage;
			string path = servers.v_serv[i].v_location[j].path;
			if (path.find("/../") != string::npos || path.find("/..") != string::npos ||
				path.find("../") != string::npos)
				throw (logic_error("Error: errpage can't contain .. : " + path));
			if (path[path.size() - 1] != '/')
				servers.v_serv[i].v_location[j].path += "/";
			if (stage == 0)
				fill_location(servers.v_serv[i].v_location[j].conf_location, servers.v_serv[i].conf_serv);
			else {
				for (int k = j; k >= 0; k--) {
					if (servers.v_serv[i].v_location[k].stage < stage) {
						stage = k;
						break;
					}
				}
				fill_location(servers.v_serv[i].v_location[j].conf_location, servers.v_serv[i].v_location[stage].conf_location);
				MULTIMAP::iterator it = servers.v_serv[i].v_location[stage].conf_location.find("root");
				if (it != servers.v_serv[i].v_location[stage].conf_location.end())
					root = it->second;
			}
			pars_manager(servers.v_serv[i].v_location[j], tmp, path, root);
		}
	}
	check_double(ports);
}
