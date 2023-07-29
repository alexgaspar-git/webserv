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

int getExtension(std::string const path) {
    std::string ext;
    std::string::size_type qmark = path.find("?");
    if (qmark != std::string::npos)
        return getExtension(path.substr(0, qmark));
    std::string::size_type dot = path.find(".");
    if (dot == std::string::npos)
        return -1;
    ext = path.substr(dot, path.size());
    if (!ext.compare(".py"))
        return PY; 
    if (!ext.compare(".php"))
        return PHP;
    if (!ext.compare(".html"))
        return HTML;
    return OTHER;
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

void closer(int pipeA, int pipeB, int pipeC, int pipeD) {
    if (pipeA)
        close(pipeA);
    if (pipeB)
        close(pipeB);
    if (pipeC)
        close(pipeC);
    if (pipeD)
        close(pipeD);
}