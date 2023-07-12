
#ifndef CONFIG_PARSER_HPP
 #define CONFIG_PARSER_HPP

# include <iostream>
# include <fstream>

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
