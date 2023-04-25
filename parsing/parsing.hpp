#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <dirent.h>
#include <cstdlib>

using namespace std;

typedef multimap<string, string> MULTIMAP;

struct block_location {
	vector<block_location> serv;
	MULTIMAP conf;
	string path;
	int stage;
};

struct block_serv {
	vector<block_location> serv;
	MULTIMAP conf;
};

struct data {
	vector<block_serv> serv;
};

void pars_conf(string & file, data & servers);
void pars_struct(data & servers);
void fill_child_location(block_location & location, block_location & old);
void fill_location(block_location & location, MULTIMAP & conf);
void pars_dir(string path, MULTIMAP & copy);
void pars_file(string path, MULTIMAP & copy, string & root);
void pars_listen(MULTIMAP & copy);
void pars_methods(MULTIMAP & copy);
void pars_errpage(MULTIMAP & copy, string & root);
