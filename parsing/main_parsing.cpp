#include "parsing/parsing.hpp"

#include <iostream>
#include <string>

int	main(int ac, char **av)
{
	if (ac != 2)
		return 1;
	string file = av[1];
	data servers;
	try {
		pars_conf(file, servers);	
		pars_struct(servers);
	}catch (std::exception & e) {
		cout << e.what() << endl;
	}
	return (0);
}
