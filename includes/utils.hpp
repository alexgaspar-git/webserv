#ifndef UTILS_HPP
#define UTILS_HPP

#include <unistd.h>
#include <string>
#include <iostream>
#include <sstream>

std::string extractPathString(const std::string &inputString);
std::string extractQueryString(const std::string &inputString);
std::string::size_type findSecondOccurrence(const std::string& str, char target);
int isCGI(std::string const path);
std::string intToString(int value);
std::string getStatusCode(int err);
std::string cleanLine(std::string &line);

#endif