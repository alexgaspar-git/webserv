#ifndef REQUESTHANDLER_HPP
#define REQUESTHANDLER_HPP

#include <iostream>
#include <map>
#include <vector>
#include <sstream>
#include <string>
#include <fstream>

#define HTTPVER "HTTP/1.1 "
#define SITEPATH "www"
#define ERRORPATH "www/error/"

class requestHandler {
private:
    std::map<std::string, std::string>  _req;
    requestHandler();
    requestHandler(requestHandler const &obj);
    requestHandler &operator=(requestHandler const &obj);
public:
    requestHandler(std::string const request);
    ~requestHandler();
    void getFirstLine(std::string const line);
    std::map<std::string, std::string> getMap();
    std::string handleRequest();
    std::string handleGet();
    std::string makeGetResponse();
    std::string makeErrorResponse(int err);
};

#endif
