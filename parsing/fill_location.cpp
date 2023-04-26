#include "parsing.hpp"

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
	fill_not_in("root", current, copy);
	fill_not_in("index" , current, copy);
	fill_not_in("methods", current, copy);
	fill_not_in("errpage", current, copy);
	fill_not_in("listen", current, copy);
}
