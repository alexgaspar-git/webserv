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

std::string::size_type findSecondOccurrence(const std::string& str, char target) {
    std::string::size_type firstIndex = std::string::npos;
    std::string::size_type secondIndex = std::string::npos;

    for (std::string::size_type i = 0; i < str.length(); i++) {
        if (str[i] == target) {
            if (firstIndex == std::string::npos)
                firstIndex = i;
            else {
                secondIndex = i;
                break;
            }
        }
    }
    return secondIndex;
}

int isCGI(std::string const path) {
    std::string test = path.substr(0, findSecondOccurrence(path, '/'));
    return test.compare("/cgi-bin");
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