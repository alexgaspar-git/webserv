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
    std::string::size_type dot = path.find_last_of(".");
    if (dot == std::string::npos)
        return -1;
    ext = path.substr(dot, path.size());
    if (!ext.compare(".py"))
        return PY; 
    if (!ext.compare(".php"))
        return PHP;
    if (!ext.compare(".html"))
        return HTML;
    if (!ext.compare(".jpg"))
        return JPEG;
    return OTHER;
}

std::string intToString(int value) {
    std::stringstream ss;
    ss << value;
    return ss.str();
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

bool isError(std::string body) {
    return body[0] == '$' && body[1] == '#';
}

std::string generateCookieName(std::map<std::string, int> cookie) {
    std::string tmp;
    std::string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    int len = charset.size();
    for (int x = 0; x < 10; x++) {
    	int i = rand() % len;
        tmp += charset[i];
    }
    if (cookie.count(tmp) != 0) {
        tmp = generateCookieName(cookie);
    }
    return (tmp);
}