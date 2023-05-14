#ifndef PARSING_HPP
# define PARSING_HPP
 
# include <iostream>
# include <string>
# include <vector>
# include <map>
# include <fstream>
# include <cstring>
# include <algorithm>
# include <dirent.h>
# include <cstdlib>

# define NB_OPT 10
# define DELIMITER "\n\r\t "

using namespace std;

typedef multimap<string, string> MULTIMAP;

struct block_location {
	MULTIMAP conf_location;
	string path;
	int stage;
};

struct block_serv {
	vector<block_location> v_location;
	MULTIMAP conf_serv;
};

struct data {
	vector<block_serv> v_serv;
};

void pars_autoindex(MULTIMAP & copy);
void pars_cgi(MULTIMAP & copy);
void pars_rewrite(MULTIMAP & copy, string & root);
void pars_conf(string & file, data & servers);
void pars_struct(data & servers);
void fill_location(MULTIMAP & current, MULTIMAP & conf);
void pars_dir(string path, MULTIMAP & copy);
void pars_file(string path, MULTIMAP & copy, string & root);
void pars_listen(MULTIMAP & copy, vector<int> & ports);
void pars_methods(MULTIMAP & copy);
void pars_errpage(MULTIMAP & copy, MULTIMAP & current, string & root);
void pars_body_size(MULTIMAP & copy);

#endif
