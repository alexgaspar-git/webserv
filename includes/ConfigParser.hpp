#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP


# include <iostream>
# include <string>   
# include <fstream>   
# include <map>
# include <sstream>
# include <vector>

# define WHITESPACE " \t\n\v\f\r"
# define LISTEN 1
# define SERVER_NAME 2
# define BODY_SIZE 3
# define AUTO_INDEX 4
# define METHOD 5
# define ERROR_PAGE 6
# define ROOT 7
# define INDEX 8
# define LOCATION 9
# define REDIRECT 10

typedef struct s_default {
	std::string root;
	std::string index;
	std::string tmp_location;
	std::map<std::string, std::string> error;
} t_default;

typedef struct s_location {
	std::string autoindex;
	std::string root;
	std::string index;
	std::string method;
	std::map<std::string, std::string> error;
	std::map<std::string, std::string> redirect;
} t_location;

typedef struct s_conf {
	std::string port;
	std::string name;
	size_t body_size;
	std::map<std::string, int> cookie;
	std::map<std::string, s_location> location;
} t_conf;

class ConfigParser
{
	private:
		s_conf *_conf_file;
		s_default *_default_conf;
		size_t _len;
		size_t _len2;
		
		int create_conf(std::string serv, int check);
		int create_conf_location(std::string serv, int check, s_location *tmp_location);
		int check_server(std::string serv);
		int check_result(std::string serv, int check);
		int handle_location(std::string serv);
		int check_option(std::string option);
		void fill_location(s_location *tmp_location);
		int check_root();
		int check_port();
		void clear_conf();
	public:
		ConfigParser();
		~ConfigParser();

		int check_conf(std::string conf);

		std::vector<s_conf> *_config;
};

void print_conf(ConfigParser *pars);

#endif