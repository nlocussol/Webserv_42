#include <vector>
#include <cstring>
#include <sstream>

std::string itostr(int i)
{
	std::stringstream ss;
	ss << i;
	return ss.str();
}

char** vector_to_c_array(const std::vector<std::string>& queryArg)
{
	char **env = new char*[queryArg.size() + 1];
	size_t queryArgSize = queryArg.size();
	size_t i;

	for (i = 0; i < queryArgSize ; i++) {
		env[i] = const_cast<char*>(queryArg[i].c_str());
	}
	env[i] = NULL;
	return env;
}

std::vector<std::string> string_to_vector(std::string bodyContent)
{
	std::vector<std::string> vector;
	std::string arg;
	size_t ampersandPos;
	while (bodyContent.length() != 0) {
		if (bodyContent.find("&") != std::string::npos)
			ampersandPos = bodyContent.find_first_of("&");
		else
			ampersandPos = bodyContent.length();
		arg = bodyContent.substr(0, ampersandPos);
		vector.push_back(arg);
		bodyContent.erase(0, ampersandPos + 1);
	}
	return vector;
}

int checkHexa(std::string& line, std::string hexa)
{
	for (int i = 0; line[i + 1]; i++) {
		if (!strchr(hexa.c_str(), line[i]))
			return (-1);
	}
	unsigned int x;   
  std::stringstream ss;
	ss << std::hex << line;
	ss >> x;
	return (x);
}
