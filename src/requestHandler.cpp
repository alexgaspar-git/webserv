#include "../includes/requestHandler.hpp"

requestHandler::requestHandler(std::string const request) {
    std::istringstream iss(request);
    std::string line;
    bool isFirstLine = true;
    while (std::getline(iss, line)) {
        if (isFirstLine) {
            getFirstLine(line);
            isFirstLine = false;
            continue;
        }
        if (line.size() != 1) {
            std::string::size_type colonPos = line.find(":");
            std::string key = line.substr(0, colonPos);
            std::string val = line.substr(colonPos + 1);
            _req[key] = val;
        }
    }
}

requestHandler::~requestHandler() {}

void requestHandler::getFirstLine(std::string const line) {
    std::string keys[3] = {"method", "path", "version"};
    std::istringstream iss(line);
    std::string word;
    int i = 0;
    while (iss >> word) {
        _req[keys[i]] = word;
        if (keys[i] == "path" && word == "/")
            _req[keys[i]] += "index.html";
        i++;
    }
}

std::map<std::string, std::string> requestHandler::getMap() { 
    return _req;
}

std::string requestHandler::handleGet() {
    std::string response = makeGetResponse();
    if (response.empty()) {
        response = makeErrorResponse();
    }
    return response;
}

std::string requestHandler::makeGetResponse() {
    std::ifstream input(SITEPATH + _req["path"]);
    if (!input.is_open()) {
        return "";
    }
    std::string result = "HTTP/1.1 200 OK\r\n";
    std::string content((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
    size_t contentLen = content.size();
    result += "Content-Length: " + std::to_string(contentLen) + "\r\n\r\n";
    result += content;
    return result;
}

std::string requestHandler::makeErrorResponse() {
    std::ifstream input("www/error/404.html");
    std::string errorResponse = "HTTP/1.1 404 Not Found\r\n";
    std::string content((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
    size_t contentLen = content.size();
    errorResponse += "Content-Length: " + std::to_string(contentLen) + "\r\n\r\n";
    errorResponse += content;
    return errorResponse;
}


