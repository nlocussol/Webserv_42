#include "parsing/parsing.hpp"

#define NB_OPT 7
#define DELIMITER "\n\r\t "

bool find_bracket(vector<string> & tab) {
	for (unsigned long i = 0; i < tab.size(); i++) {
		for (unsigned long j = 0; j < tab[i].size(); j++) {
			if (tab[i][j] == '{' || tab[i][j] == '}')
				return true;
		}
	}
	return false;
}

vector<string> mysplit(string & line, string delimiter) {
	string str;
	vector<string> tab;
	for (unsigned long i = 0; i < line.size(); i++) {
		while (line[i] && strchr(delimiter.c_str(), line[i]))
			i++;
		while (line[i] && strchr(delimiter.c_str(), line[i])) {
			str += line[i];
			i++;
		}
		tab.push_back(str);
		str.clear();
	}
	return (tab);
}

void recursive_location(fstream & file, block_serv & servers, vector<string> options) {
	string line;
	vector<string> tab = mysplit(line, DELIMITER);
	if (!getline(file, line) || tab.size() > 1 || tab[0] != "{")
		throw (logic_error("Error: need a bracket after location block!"));
	block_location location;
	servers.serv.push_back(location);
	int index = servers.serv.size() - 1;
	vector<string>::iterator it;
	while (getline(file, line)) {
		if (!line.empty()) {
			vector<string> tab = mysplit(line, DELIMITER);
			it = find(options.begin(), options.end(), tab[0]);
		}
		if (line.empty())
			continue ;
		else if (tab.size() == 1 && tab[0] == "}")
			return ;
		else if (find_bracket(tab))
			throw (logic_error("Error: a bracket are a bad position in file!"));
		else if (tab.size() == 2 && tab[0] == "location")
			recursive_location(file, servers, options);
		else if (it != options.end()) {
			if (tab.size() < 2)
				throw (logic_error("Error: " + tab[0] + " need an argument!"));
			if (tab[0] == "errpage" && tab.size() != 3)
				throw (logic_error("Error: errpage need two arguments!"));
			for (unsigned long i = 1; i < tab.size(); i++)
				servers.serv[index].conf.insert(make_pair(tab[0], tab[1]));
		} 
		else
			throw (logic_error("Error: bad input!"));
	}
	throw (logic_error("Error: need a close bracket at the end of the server!"));
}

void pars_line(fstream & file, data & servers, vector<string> options) {
	string line;
	vector<string> tab = mysplit(line, DELIMITER);
	if (!getline(file, line) || tab.size() > 1 || tab[0] != "{")
		throw (logic_error("Error: need a bracket after server block!"));
	block_serv serv;
	servers.serv.push_back(serv);
	int index = servers.serv.size() - 1;
	vector<string>::iterator it;
	while (getline(file, line)) {
		if (!line.empty()) {
			vector<string> tab = mysplit(line, DELIMITER);
			it = find(options.begin(), options.end(), tab[0]);
		}
		if (line.empty())
			continue ;
		else if (tab.size() == 1 && tab[0] == "}")
			return ;
		else if (find_bracket(tab))
			throw (logic_error("Error: a bracket are a bad position in file!"));
		else if (tab.size() == 2 && tab[0] == "location")
			recursive_location(file, servers.serv[index], options);
		else if (it != options.end()) {
			if (tab.size() < 2)
				throw (logic_error("Error: " + tab[0] + " need an argument!"));
			if (tab[0] == "errpage" && tab.size() != 3)
				throw (logic_error("Error: errpage need two arguments!"));
			for (unsigned long i = 1; i < tab.size(); i++)
				servers.serv[index].conf.insert(make_pair(tab[0], tab[1]));
		} 
		else
			throw (logic_error("Error: bad input!"));
	}
	throw (logic_error("Error: need a close bracket at the end of the server!"));
}

void pars_conf(string & file, data & servers) {
	string opt[NB_OPT] = {"root", "index", "listen", ""};
	vector<string> options;
	for (int i = 0; i < NB_OPT; i++)
		options.push_back(opt[i]);
	fstream input;
	input.open(file.c_str(), ios::in);
	if (!input)
		throw (logic_error("Error: config file can't be open!"));
	string line;
	vector<string> tab = mysplit(line, DELIMITER);
	while (getline(input, line)) {
		if (line.empty())
			continue ;
		else if (tab.size() == 1 && tab[0] == "server") 
			pars_line(input, servers, options);
		else
			throw (logic_error("Error: need a server block!"));
	}
}
