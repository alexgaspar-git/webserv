#ifndef UTILS_HPP
#define UTILS_HPP

#include <unistd.h>
#include <string>
#include <iostream>
#include <sstream>
#include "requestHandler.hpp"

std::string extractPathString(const std::string &inputString);
std::string extractQueryString(const std::string &inputString);
std::string intToString(int value);
std::string cleanLine(std::string &line);
int getExtension(std::string const path);
void closer(int pipeA, int pipeB, int pipeC, int pipeD);
bool isError(std::string body);
std::string generateCookieName(std::map<std::string, int> cookie);


#endif