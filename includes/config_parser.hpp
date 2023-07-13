
#ifndef CONFIG_PARSER_HPP
 #define CONFIG_PARSER_HPP

# include <iostream>
# include <fstream>
# include <vector>
# include <map>

	// std::string tmp = "GET POST";
	// if (tmp.find("POST") != (size_t)-1)
	// 	std::cout << "find Post" << std::endl;
	// if (tmp.find("GET") != (size_t)-1)
	// 	std::cout << "find get" << std::endl;
	// if (tmp.find("oseefikgheog") == (size_t)-1)
	// 	std::cout << "find nothing" << std::endl;
	// peut utiliser un truc dans le genre si pas vector

typedef struct t_location {
	std::string index;
	std::vector<std::string> method;//check le mieux entre vector ou utiliser un find avec une string
	std::vector<std::string> error;
} s_location;

typedef struct t_conf {
	std::string port;
	std::string name;
	std::string root;
	std::string autoindex = "off";
	std::map<std::string, s_location> location;
} s_conf;

class config_parser
{
	private:

	public:
		config_parser(std::string conf_file);
		config_parser(config_parser const &src);
		~config_parser();

		config_parser &operator=(config_parser const &rhs);
};

#endif
