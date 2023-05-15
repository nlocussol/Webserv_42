#include "../inc/Server.hpp"
#include "../inc/parsing.hpp"

void	handle_sigint(int signum)
{
	(void)signum;
	if (Server::_running == true)
		Server::_running = false;
}

int main (int ac, char *av[])
{
	string file;
	if (ac > 2) {
		std::cerr << "Error: Correct format is ./webserv [configuration file](optionnal)\n";
		return EXIT_FAILURE;
	}
	if (ac == 2)
		file = av[1];
	else
		file = "conf/default.conf";
	data servers;
	// Singleton allocation init
	try {
		pars_conf(file, servers);	
		pars_struct(servers);
	}catch (std::exception & e) {
		std::cerr << e.what() << endl;
		return (EXIT_FAILURE);
	}
  	Server server(servers);
	try {
		server.setSocket();
		server.runServer();
	} catch (std::exception & e) {
		std::cerr << e.what() << std::endl;
		return (1);
	}
  	return 0;
}
