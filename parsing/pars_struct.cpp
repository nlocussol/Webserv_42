#include "parsing.hpp"

void pars_manager(block_serv & servers) {
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
			if (j == 0)
				fill_location(servers.serv[i].serv[0], servers.serv[i].conf);
			else {
				int stage = servers.serv[i].serv[j].stage;
				for (int k = j; k >= 0; k--) {
					if (servers.serv[i].serv[k].stage < stage) {
						stage = k;
						break;
					}
				}
				fill_child_location(servers.serv[i].serv[j], servers.serv[i].serv[stage]);
			}
		}
	}
}
