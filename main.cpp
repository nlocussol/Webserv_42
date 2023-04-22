#include "Server.hpp"
#include <cstdlib>
#include <iostream>
#include <map>

// int parseConfigFile(std::multimap<std::string, std::string>& config)
// {
//   int nbServ = 1; //nbServ should be determine by parsing config file
//   config.insert(std::make_pair("port", "16161"));
//   return nbServ;
// }

int main (int argc, char *argv[])
{
  (void)argv;
  if (argc > 2) {
    std::cerr << "Error: Correct format is ./webserv [configuration file](optionnal)\n";
    return EXIT_FAILURE;
  }
  //Maybe do a pre-parsing to see how many servs we need to launch, so we can create adequate nb of multimaps
  // std::multimap<std::string, std::string> config;
  // int nbServ = parseConfigFile(config);
  // Add some tests to see if server(s) are usable, else quit
  Server server;
  server.setSocket();
  server.runServer();
  return 0;
}
