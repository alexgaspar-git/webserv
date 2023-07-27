#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP


# include <iostream>
# include <string>   
# include <fstream>   
# include <map>
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
# define UPLOAD 10

typedef struct s_default {
	std::string autoindex;
	std::string root;
	std::string index;
	std::string method;
	std::string tmp_location;
	std::map<std::string, std::string> error;
} t_default;

typedef struct s_location {
	std::string autoindex;
	std::string root;
	std::string index;
	std::string method;
	std::string upload;
	std::map<std::string, std::string> error;
} t_location;

typedef struct s_conf {
	std::string port;
	std::string name;
	std::string body_size;
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
		void clear_conf();
	public:
		std::vector<s_conf> *_config;
		ConfigParser();
		ConfigParser(ConfigParser const &src);
		~ConfigParser();

		ConfigParser &operator=(ConfigParser const &rhs);

		int check_conf(std::string conf);
};

void print_conf(ConfigParser *pars);

#endif