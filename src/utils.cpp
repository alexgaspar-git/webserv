#include "utils.hpp"

std::string cleanLine(std::string &line) {
    std::string::size_type pos = line.find("\r");
    if (pos == std::string::npos) {
        return line += '\n';
    }
    std::string str = line.substr(0, pos);
    return str;
}

std::string extractPathString(const std::string &inputString) {
    size_t queryEnd = inputString.find('?');
    return inputString.substr(0, queryEnd);
}

std::string extractQueryString(const std::string &inputString) {
    size_t queryStart = inputString.find('?');
    if (queryStart != std::string::npos) {
        return inputString.substr(queryStart + 1);
    }
    return "";
}

int isCGI(std::string const path) {
    size_t pos = path.find_last_of('.');
    std::string test = path.substr(pos);
    if (test.compare(".py") == 0)
        return PYTHON;
    if (test.compare(".php") == 0)
        return PHP;
    return NOCGI;
}

std::string intToString(int value) {
    std::stringstream ss;
    ss << value;
    return ss.str();
}

std::string getStatusCode(int err) {
    switch (err) {
        case 200:
            return "200 OK";
        case 404:
            return "404 Not Found";
        default:
            return "?";
    }
}