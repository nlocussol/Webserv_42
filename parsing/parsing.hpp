#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <cstring>
#include <algorithm>

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
