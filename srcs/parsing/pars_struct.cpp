#include "../../inc/parsing.hpp"

template<typename T>
void pars_manager(T & servers) {
	MULTIMAP::iterator it = servers.conf.find("root");
	MULTIMAP copy = servers.conf;

	pars_dir("root", copy);
	pars_file("index", copy, it->second);
	pars_listen(copy);
	pars_methods(copy);
	pars_errpage(copy, it->second);
}

void pars_struct(data & servers) {
	for (unsigned long i = 0; i < servers.serv.size(); i++) {
		pars_manager(servers.serv[i]);
		for (unsigned long j = 0; j < servers.serv[i].serv.size(); j++) {
			int stage = servers.serv[i].serv[j].stage;
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
			pars_manager(servers.serv[i].serv[j]);
		}
	}
}
