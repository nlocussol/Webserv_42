#include "../../inc/parsing.hpp"
#include <stdexcept>

void check_bad_opt(MULTIMAP current) {
	MULTIMAP::iterator it = current.find("listen");
	if (it != current.end())
		throw (logic_error("Error: listen can't be defined in location block!"));
	it = current.find("server_names");
	if (it != current.end())
		throw (logic_error("Error: server_names can't be defined in location block!"));
	it = current.find("limit_client_body_size");
	if (it != current.end())
		throw (logic_error("Error: limit_client_body_size can't be defined in location block!"));
}

void fill_not_in(string path, MULTIMAP & current, MULTIMAP & copy) {
	MULTIMAP::iterator it_current = current.find(path);
	MULTIMAP::iterator it_copy = copy.find(path);
	if (it_current != current.end() || it_copy == copy.end())
		return;
	while (it_copy != copy.end()) {
		current.insert(make_pair(it_copy->first, it_copy->second));
		copy.erase(it_copy);
		it_copy = copy.find(path);
	}
} 

void fill_location(MULTIMAP & current, MULTIMAP & conf) {
	MULTIMAP copy = conf;
	check_bad_opt(current);
	fill_not_in("root", current, copy);
	fill_not_in("index" , current, copy);
	fill_not_in("methods", current, copy);
	fill_not_in("errpage", current, copy);
	fill_not_in("listen", current, copy);
}
