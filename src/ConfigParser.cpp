#include "ConfigParser.hpp"

ConfigParser::ConfigParser() {
	this->_config = new std::vector<s_conf>;
	this->_conf_file = new s_conf;
	this->_default_conf = new s_default;
}

ConfigParser::~ConfigParser() {
	delete this->_config;
	delete this->_conf_file;
	delete this->_default_conf;
}

int ConfigParser::check_result(std::string serv, int check) {
	if (check != LOCATION)
		this->_len = serv.find_first_of(";", this->_len);
	else
		this->_len = serv.find_first_of("{", this->_len);
	if (check != ERROR_PAGE && check != METHOD && check != LOCATION && check != REDIRECT) {
		size_t tmp = serv.find_first_of(" ", this->_len2);
		if (tmp < this->_len){
			std::cerr << "space in option" << std::endl;
			return (1);
		}
	}
	else if (check == ERROR_PAGE || check == REDIRECT){
		size_t tmp = serv.find_first_of(" ", this->_len2);
		if (tmp == std::string::npos || tmp > this->_len) {
			std::cerr << "not enough space in option" << std::endl;
			return (1);
		}	
		tmp = serv.find_first_of(" ", tmp + 1);
		if (tmp < this->_len){
			std::cerr << "too many space in option" << std::endl;
			return (1);
		}
	}
	else if (check == METHOD){
		int tmp_len = this->_len2;
		for (int x = 0; x < 4; x++) {
			if (x == 3) {
				std::cerr << "too many space in option" <<std::endl;
				return (1);
			}
			size_t tmp = serv.find_first_of(" ", tmp_len);
			if (tmp == std::string::npos || tmp > this->_len) {
				std::string tmp2 = serv.substr(tmp_len, this->_len - tmp_len);
				if (tmp2 != "GET" && tmp2 != "POST" && tmp2 != "DELETE") {
					std::cerr << "wrong method" <<std::endl;
					return (1);
				}
				return (0);
			}
			else {
				if (serv[tmp + 1] && serv[tmp + 1] == ';') {
					std::cerr << "space in option" << std::endl;
					return (1);
				}
				std::string tmp2 = serv.substr(tmp_len, tmp - tmp_len);
				if (tmp2 != "GET" && tmp2 != "POST" && tmp2 != "DELETE") {
					std::cerr << "wrong method" <<std::endl;
					return (1);
				}
				tmp_len = tmp + 1;
			}
		}
	}
	else {
		size_t tmp = serv.find_first_of(" ", this->_len2);
		if (tmp == std::string::npos || tmp > this->_len) {
			std::cerr << "not enough space in option" << std::endl;
			return (1);
		}	
		this->_default_conf->tmp_location = serv.substr(this->_len2, tmp - this->_len2);
		tmp = serv.find_first_of(" ", tmp + 1);
		if (tmp < this->_len){
			std::cerr << "too many space in option" << std::endl;
			return (1);
		}
	}
	return (0);
}

size_t check_body_size(std::string port) {
	size_t i = 0;
	for ( std::string::iterator it=port.begin(); it!=port.end(); ++it) {
		if (!isdigit(port[i++])) {
			std::cerr<< "only digit are expted for body_size" << std::endl;
			return (0);
		}
	}
	std::istringstream iss(port);
	size_t number = 0;
	iss >> number;
	if (number == 0 || number > 5000000) {
		std::cerr<< "values accepted for body_size are between 1 and 5000000" << std::endl;
		return (0);
	}
	return (number);
}


int ConfigParser::create_conf(std::string serv, int check) {
	if (check_result(serv, check)) {
		std::cerr << "error in server conf :" << std::endl;
		return (1);
	}
	switch (check) {
		case LISTEN:
			if (this->_conf_file->port.empty())
				this->_conf_file->port = serv.substr(this->_len2, this->_len - this->_len2);
			else {
				std::cerr << "only one port per serv" << std::endl;
				return (1);
			}
			break;
		case SERVER_NAME:
			if (this->_conf_file->name.empty())
				this->_conf_file->name = serv.substr(this->_len2, this->_len - this->_len2);
			else {
				std::cerr << "only one name per serv" << std::endl;
				return (1);
			}
			break;
		case BODY_SIZE:
			if (this->_conf_file->body_size == 0) {
				if (!(this->_conf_file->body_size = check_body_size(serv.substr(this->_len2, this->_len - this->_len2))))
					return (1);
			}
			else {
				std::cerr << "only one body_size per serv" << std::endl;
				return (1);
			}
			break;
		case ERROR_PAGE:
		{
			size_t tmp = serv.find_first_of(" ", this->_len2);
			std::string code = serv.substr(this->_len2, tmp - this->_len2);
			tmp++;
			this->_default_conf->error.insert(std::pair<std::string, std::string>(code, serv.substr(tmp, this->_len - tmp)));
		}
			break;
		case ROOT:
			if (this->_default_conf->root.empty())
				this->_default_conf->root = serv.substr(this->_len2, this->_len - this->_len2);
			else {
				std::cerr << "error : multiple root option in serv conf" << std::endl;
				return (1);
			}
			break;
		case INDEX:
			if (this->_default_conf->index.empty())
				this->_default_conf->index = serv.substr(this->_len2, this->_len - this->_len2);
			else {
				std::cerr << "error : multiple index option in serv conf" << std::endl;
				return (1);
			}
			break;
		case LOCATION:
			{
				if (handle_location(serv))
				{
					std::cerr << "error in location part" << std::endl;
					return (1);
				}
			}
			break;
		default:
		{
			std::cerr << "invalid option in serv conf" << std::endl;
			return (1);
		}
	}
	return (0);
}

int ConfigParser::create_conf_location(std::string serv, int check, s_location *tmp_location) {
	if (check_result(serv, check)) {
		std::cerr << "error in location part :" << std::endl;
		return (1);
	}
	switch (check) {
		case AUTO_INDEX:
			if (tmp_location->autoindex.empty())
				tmp_location->autoindex = serv.substr(this->_len2, this->_len - this->_len2);
			else {
				std::cerr << "error : multiple autoindex option in location part" << std::endl;
				return (1);
			}
			if (!(tmp_location->autoindex.empty()) && tmp_location->autoindex != "off" && tmp_location->autoindex != "on") {
				std::cerr << "autoindex can only be 'on' or 'off'" << std::endl;
				return (1);
			}
			break;
		case METHOD:
			if (tmp_location->method.empty())
				tmp_location->method = serv.substr(this->_len2, this->_len - this->_len2);
			else {
				std::cerr << "error : multiple method option in location part" << std::endl;
				return (1);
			}
			break;
		case ERROR_PAGE:
		{
			size_t tmp = serv.find_first_of(" ", this->_len2);
			std::string code = serv.substr(this->_len2, tmp - this->_len2);
			tmp++;
			tmp_location->error.insert(std::pair<std::string, std::string>(code, serv.substr(tmp, this->_len - tmp)));
		}
			break;
		case REDIRECT:
		{
			if (tmp_location->redirect.empty()) {
				size_t tmp = serv.find_first_of(" ", this->_len2);
				std::string code = serv.substr(this->_len2, tmp - this->_len2);
				tmp++;
				tmp_location->redirect.insert(std::pair<std::string, std::string>(code, serv.substr(tmp, this->_len - tmp)));
			}
			else {
				std::cerr << "only one return per location is accepted" << std::endl;
				return (1);
			}
		}
			break;
		case ROOT:
			if (tmp_location->root.empty())
				tmp_location->root = serv.substr(this->_len2, this->_len - this->_len2);
			else {
				std::cerr << "error : multiple root option in location part" << std::endl;
				return (1);
			}
			break;
		case INDEX:
			if (tmp_location->index.empty())
				tmp_location->index = serv.substr(this->_len2, this->_len - this->_len2);
			else {
				std::cerr << "error : multiple index option in location part" << std::endl;
				return (1);
			}
			break;
		default:
		{
			std::cout << check << std::endl;
			std::cerr << "invalide option in location" << std::endl;
			return (1);
		}
	}
	return (0);
}

void ConfigParser::fill_location(s_location *tmp_location) {
	if (tmp_location->autoindex.empty())
		tmp_location->autoindex = "off";
	if (tmp_location->root.empty())
		tmp_location->root = this->_default_conf->root;
	for (std::map<std::string, std::string>::iterator it = this->_default_conf->error.begin(); it != this->_default_conf->error.end(); it++) {
		if (tmp_location->error.find(it->first) == tmp_location->error.end())
			tmp_location->error.insert(std::pair<std::string, std::string>(it->first, it->second));
	}
}

int ConfigParser::check_option(std::string option) {
    std::map<std::string, int> list;
    list.insert(std::pair<std::string, int>("listen", LISTEN));
    list.insert(std::pair<std::string, int>("server_name", SERVER_NAME));
    list.insert(std::pair<std::string, int>("body_size", BODY_SIZE));
    list.insert(std::pair<std::string, int>("autoindex", AUTO_INDEX));
    list.insert(std::pair<std::string, int>("method", METHOD));
    list.insert(std::pair<std::string, int>("error_page", ERROR_PAGE));
    list.insert(std::pair<std::string, int>("root", ROOT));
    list.insert(std::pair<std::string, int>("index", INDEX));
    list.insert(std::pair<std::string, int>("location", LOCATION));
    list.insert(std::pair<std::string, int>("return", REDIRECT));

    if (list.find(option) == list.end())
		return (0);
    else
		return (list[option]);
}

int ConfigParser::handle_location(std::string serv) {
	this->_len++;
	s_location tmp_loc;
	size_t end = serv.find_first_of("}", this->_len);
	if (end == std::string::npos) {
		std::cerr << "bad end" << std::endl;
		return (1);
	}
	serv.resize(end + 1);
	while (this->_len < end) {
		this->_len = serv.find_first_not_of(WHITESPACE, this->_len);
		if (this->_len == end)
			break;
		this->_len2 = serv.find_first_of(WHITESPACE, this->_len);
		int check = check_option(serv.substr(this->_len, this->_len2 - this->_len));
		if (!check) {
			std::cerr << "unknown option" << std::endl;
			return (1);
		}
		else {
			this->_len2++;
			if (create_conf_location(serv, check, &tmp_loc)) {
				std::cerr << "error create_conf" << std::endl;
				return (1);
			}
			this->_len++;
		}
	}
	fill_location(&tmp_loc);
	this->_conf_file->location.insert(std::pair<std::string, s_location>(this->_default_conf->tmp_location, tmp_loc));
	return (0);
}

int ConfigParser::check_server(std::string serv) {
	this->_len = 0;
	size_t end = serv.find_last_of("}");
	if (end == std::string::npos || end != serv.find_last_not_of(WHITESPACE)) {
		std::cerr << "bad end" << std::endl;
		return (1);
	}
	serv.resize(end + 1);
	while (this->_len < end) {
		this->_len = serv.find_first_not_of(WHITESPACE, this->_len);
		if (this->_len == end)
			break;
		this->_len2 = serv.find_first_of(WHITESPACE, this->_len);
		int check = check_option(serv.substr(this->_len, this->_len2 - this->_len));
		if (!check) {
			std::cerr << "unknown option" << std::endl;
			return (1);
		}
		else {
			this->_len2++;
			if (create_conf(serv, check)) {
				std::cerr << "error create_conf" << std::endl;
				return (1);
			}
			this->_len++;
		}
	}
	return (0);
}

int ConfigParser::check_root() {
	for (std::vector<s_conf>::iterator it = this->_config->begin(); it != this->_config->end(); it++) {
		bool tmp = 0;
		for (std::map<std::string, s_location>::iterator m = it->location.begin(); m != it->location.end(); m++) {
			if (m->first == "/")
				tmp = 1;
		}
		if (!tmp) {
			std::cerr << "server need location root -> '/' to work" << std::endl;
			return (1);
		}
		if (it->port.empty()){
			std::cerr << "server need a port to work" << std::endl;
			return (1);
		}
	}
	return (0);
}

int ConfigParser::check_port() {
	for (std::vector<s_conf>::iterator it = this->_config->begin(); it != this->_config->end(); it++) {
		std::string tmp = it->port;
		for (std::vector<s_conf>::iterator itt = it + 1; itt != this->_config->end(); itt++) {
			if (tmp == itt->port) {
				std::cerr << "repeated port forbidden" << std::endl;
				return (1);
			}
		}
	}
	return (0);
}

int ConfigParser::check_conf(std::string conf) {
	size_t len_max = conf.length();
	size_t len = conf.find("server {");
	size_t end = 0;
	if (len == std::string::npos) {
		std::cerr << "doesn't find any server part" << std::endl;
		return (1);
	}
	if (len != conf.find_first_not_of(WHITESPACE)) {
		std::cerr << "error something before server" << std::endl;
		return (1);
	}
	while (len_max != end) {
		clear_conf();
		end = conf.find("server {", len + 1);
		if (end == std::string::npos){
			end = len_max;
		}
		if (check_server(conf.substr(len + 8, end - (len + 8))) == 1) {
			std::cerr << "error serv part" << std::endl;
			return (1);
		}
		len = end;
		if (this->_conf_file->body_size == 0)
			this->_conf_file->body_size = 1000000;
		this->_config->push_back(*this->_conf_file);
	}
	if (this->check_root())
		return (1);
	if (this->check_port())
		return (1);
	return (0);
}

void ConfigParser::clear_conf() {
	this->_conf_file->port.clear();
	this->_conf_file->name.clear();
	this->_conf_file->body_size = 0;
	this->_conf_file->location.clear();
	this->_default_conf->root.clear();
	this->_default_conf->index.clear();
	this->_default_conf->tmp_location.clear();
	this->_default_conf->error.clear();
}

void print_conf(ConfigParser *pars){
	for (std::vector<s_conf>::iterator it = pars->_config->begin(); it != pars->_config->end(); it++) {
		std::cout << std::endl << "--------------------------------------------------" << std::endl;
		std::cout << "port :" << it->port << std::endl;
		std::cout << "name :" << it->name << std::endl;
		std::cout << "body_size :" << it->body_size << std::endl;
		for (std::map<std::string, s_location>::iterator m = it->location.begin(); m != it->location.end(); m++) {
			std::cout << std::endl << m->first << std::endl;
			std::cout << "autoindex :" << m->second.autoindex <<std::endl;
			std::cout << "root :" << m->second.root <<std::endl;
			std::cout << "index :" << m->second.index <<std::endl;
			std::cout << "method :" << m->second.method <<std::endl;
			for (std::map<std::string, std::string>::iterator ma = m->second.error.begin(); ma != m->second.error.end(); ma++)
				std::cout << "error :" << ma->first << " " << ma->second << std::endl;
			for (std::map<std::string, std::string>::iterator ma = m->second.redirect.begin(); ma != m->second.redirect.end(); ma++)
				std::cout << "redirect :" << ma->first << " " << ma->second << std::endl;
		}
	}
	std::cout <<std::endl;
}