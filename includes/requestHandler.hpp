#ifndef REQUESTHANDLER_HPP
#define REQUESTHANDLER_HPP

#include <iostream>
#include <map>
#include <sstream>
#include <string>


class requestHandler {
private:
    std::map<std::string, std::string>  _req;
    requestHandler();
    requestHandler(requestHandler const &obj);
    requestHandler &operator=(requestHandler const &obj);
public:
    requestHandler(std::string const request);
    ~requestHandler();
};

#endif
