
#include "config_parser.hpp"

int check_conf(std::string conf) {
	std::string tmp;
}

config_parser::config_parser(std::string conf_file) {
	std::ifstream input(conf_file);
    if (!input.is_open()) {
        std::cout << ".conf file doesn't exist." << std::endl;
		exit (1);//throw error
    }
    std::string conf((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
	if (check_conf(conf) == 1)
	{
		exit (1);//throw error
	}
	//creation structure
}

config_parser::config_parser(config_parser const &rhs) {}

config_parser::~config_parser() {}

config_parser &config_parser::operator=(config_parser const &rhs) {
	return (*this);
}

int check_conf(std::string conf) {
	return (0);
}