#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include <iostream>
#include <map>
#include <string>
#include <unistd.h>

#include "requestHandler.hpp"
#include "utils.hpp"

class CGIHandler {
private:
    std::map<std::string, std::string> _req;
    std::string _body;
    const char *_path;
    const char **_argv;
    const char **_env;
    const char **getArgv();
    const char **getEnv();
public:
    CGIHandler(std::map<std::string, std::string> const &request);
    ~CGIHandler();
    std::string initCGI();
    bool execCGI();
    std::string constructResponse();
};



#endif