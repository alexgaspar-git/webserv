
#ifndef CONFIG_PARSER_HPP
 #define CONFIG_PARSER_HPP

# include <iostream>
# include <fstream>
# include <map>

	// std::string tmp = "GET POST";
	// if (tmp.find("POST") != (size_t)-1)
	// 	std::cout << "find Post" << std::endl;
	// if (tmp.find("GET") != (size_t)-1)
	// 	std::cout << "find get" << std::endl;
	// if (tmp.find("oseefikgheog") == (size_t)-1)
	// 	std::cout << "find nothing" << std::endl;
	// peut utiliser un truc dans le genre si pas vector

typedef struct s_location {
	std::string autoindex;
	std::string root;
	std::string index;
	std::string method;//check le mieux entre vector ou utiliser un find avec une string
	std::map<std::string, std::string> error;
} t_location;

typedef struct s_conf {
	std::string port;
	std::string name;
	std::string body_size;//regarder si size_t meilleur option
	std::map<std::string, s_location> location;
} t_conf;

class config_parser
{
	private:

	public:
		config_parser(std::string conf_file);
		config_parser(config_parser const &src);
		~config_parser();

		config_parser &operator=(config_parser const &rhs);

		int check_conf(std::string conf);
};

#endif
