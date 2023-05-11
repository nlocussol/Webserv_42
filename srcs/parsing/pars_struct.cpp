#include "../../inc/parsing.hpp"
#include <sstream>
#include <string>

void fill_root(MULTIMAP & current) {
	MULTIMAP::iterator it = current.find("root");
	if (it->second[it->second.size() - 1] != '/')
		it->second += "/";
}

void pars_manager(block_location & servers, vector<int> & ports) {
	MULTIMAP::iterator it = servers.conf_location.find("root");
	MULTIMAP copy = servers.conf_location;

	fill_root(servers.conf_location);
	pars_dir("root", copy);
	pars_file("index", copy, it->second);
	pars_listen(copy, ports);
	pars_methods(copy);
	pars_errpage(copy, servers.conf_location, it->second);
	pars_body_size(copy);
	pars_rewrite(copy);
	pars_cgi(copy);
}

void pars_manager(block_serv & servers, vector<int> & ports) {
	MULTIMAP::iterator it = servers.conf_serv.find("root");
	MULTIMAP copy = servers.conf_serv;

	fill_root(servers.conf_serv);
	pars_dir("root", copy);
	pars_cgi(copy);
	pars_file("index", copy, it->second);
	pars_listen(copy, ports);
	pars_methods(copy);
	pars_errpage(copy, servers.conf_serv, it->second);
	pars_body_size(copy);
}

template<typename T>
void check_double(T & tab) {
	for (size_t i = 0; i < tab.size(); i++) {
		if (std::count(tab.begin(), tab.end(), tab[i]) > 1) {
			stringstream ss;
			ss << tab[i];
			throw (logic_error("Error: two serveurs can't listen on the same tab: " + ss.str()));
		}
	}
}

void pars_struct(data & servers) {
	vector<int> ports;
	for (unsigned long i = 0; i < servers.v_serv.size(); i++) {
		pars_manager(servers.v_serv[i], ports);
		for (unsigned long j = 0; j < servers.v_serv[i].v_location.size(); j++) {
			vector<int> tmp;
			int stage = servers.v_serv[i].v_location[j].stage;
			string path = servers.v_serv[i].v_location[j].path;
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
			}
			pars_manager(servers.v_serv[i].v_location[j], tmp);
		}
	}
	check_double(ports);
}
