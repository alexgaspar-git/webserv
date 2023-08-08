#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include <iostream>
#include <map>
#include <string>
#include <cstring>
#include <unistd.h>
#include <stdexcept>
#include <csignal>

#include "requestHandler.hpp"
#include "utils.hpp"

class CGIHandler {
private:
    std::map<std::string, std::string> _req;
    std::string _body;
    std::string _fileName;
    std::string _path;
    char **_env;
    std::map<std::string, int> &_cookie;
    void getEnv(std::map<std::string, int> &cookie);
public:
    CGIHandler(std::map<std::string, std::string> const &request, int type, std::map<std::string, int> &cookie);
    ~CGIHandler();
    std::string initCGI();
    bool execCGI();
    std::string constructResponse();
    void parseBody();
};



#endif