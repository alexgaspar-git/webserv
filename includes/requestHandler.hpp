#ifndef REQUESTHANDLER_HPP
#define REQUESTHANDLER_HPP

#include <iostream>
#include <map>
#include <vector>
#include <sstream>
#include <string>
#include <fstream>

#define HTTPVER "HTTP/1.1 "
#define ERRORPATH "www/error/"
#define PATH _req["path"]
#define REQBODY _req["body"]

enum Extensions {
    HTML,
    PHP,
    PY,
    OTHER
};

#include "CGIHandler.hpp"
#include "utils.hpp"
#include "ConfigParser.hpp"

class requestHandler {
private:
    std::map<std::string, std::string>  _req;
    requestHandler();
    requestHandler(requestHandler const &obj);
    requestHandler &operator=(requestHandler const &obj);
    std::vector<s_conf>::iterator _currentClient;
    std::string _sitePath;
public:
    requestHandler(std::string const request, ConfigParser *pars);
    ~requestHandler();
    void getFirstLine(std::string const &line);
    std::map<std::string, std::string> getMap();
    std::string handleRequest();
    void handlePath();
    std::string makeGetResponse();
    std::string makeErrorResponse(int err);
    std::string buildResponse(std::string const &body);
    std::string buildErrResponse(std::string const &body);
    std::string handleHTML();
    std::string constructGetResponse(int status, std::ifstream &input);
    std::vector<s_conf>::iterator getCurrentClient(ConfigParser *pars);
};

std::string getStatusCode(int err);

#endif
