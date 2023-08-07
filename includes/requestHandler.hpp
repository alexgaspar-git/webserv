#ifndef REQUESTHANDLER_HPP
#define REQUESTHANDLER_HPP

#include <iostream>
#include <map>
#include <vector>
#include <sstream>
#include <string>
#include <fstream>

#define HTTPVER "HTTP/1.1 "
#define MTD _req["method"]
#define ERRORPATH "www/error/"
#define PATH _req["path"]
#define REQBODY _req["body"]

enum Extensions {
    HTML,
    PHP,
    PY,
    JPEG,
    OTHER
};

#include "CGIHandler.hpp"
#include "utils.hpp"
#include "ConfigParser.hpp"

class requestHandler {
private:
    requestHandler();
    std::map<std::string, std::string>  _req;
    std::vector<s_conf>::iterator _currentClient;
    std::string _sitePath;
    bool _isForbidden;
    bool _isAutoIndex;
    bool _noRoot;
    bool _badRequest;
    bool _isHTTPS;
    std::string _redirectCode;
    std::string _redirectLink;
public:
    bool _noCurrentClient;
    requestHandler(std::string const request, ConfigParser *pars);
    ~requestHandler();
    bool getFirstLine(std::string const &line);
    std::string handleRequest();
    bool handlePath();
    std::string buildResponse(std::string const &body);
    std::string buildErrResponse(std::string const &body);
    std::string handleHTML(std::string const &path);
    bool defaultError(std::string const &errcode);
    bool checkMethod(const s_location &locationData);
    std::vector<s_conf>::iterator getCurrentClient(ConfigParser *pars);
    std::string const buildRedirResponse();
};

std::string getStatusCode(int err);

#endif
