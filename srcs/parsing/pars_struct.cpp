#include "../../inc/parsing.hpp"
#include <sstream>
#include <string>

void fill_root(MULTIMAP & current) {
	MULTIMAP::iterator it = current.find("root");
	while (it != current.end()) {
		if (it->second[it->second.size() - 1] != '/')
			it->second += "/";
		current.erase(it);
		it = current.find("root");
	}
}

template<typename T>
void pars_manager(T & servers, vector<int> & ports) {
	MULTIMAP::iterator it = servers.conf.find("root");
	MULTIMAP copy = servers.conf;

	fill_root(servers.conf);
	pars_dir("root", copy);
	pars_file("index", copy, it->second);
	pars_listen(copy, ports);
	pars_methods(copy);
	pars_errpage(copy, servers.conf, it->second);
	pars_body_size(copy);
	//redirect
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
	for (unsigned long i = 0; i < servers.serv.size(); i++) {
		pars_manager(servers.serv[i], ports);
		for (unsigned long j = 0; j < servers.serv[i].serv.size(); j++) {
			vector<int> tmp;
			int stage = servers.serv[i].serv[j].stage;
			string path = servers.serv[i].serv[j].path;
			if (path[path.size() - 1] != '/')
				path += "/";
			if (stage == 0)
				fill_location(servers.serv[i].serv[j].conf, servers.serv[i].conf);
			else {
				for (int k = j; k >= 0; k--) {
					if (servers.serv[i].serv[k].stage < stage) {
						stage = k;
						break;
					}
				}
				fill_location(servers.serv[i].serv[j].conf, servers.serv[i].serv[stage].conf);
			}
			pars_manager(servers.serv[i].serv[j], tmp);
		}
	}
	check_double(ports);
}
