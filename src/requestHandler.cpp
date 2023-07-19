#include "../includes/requestHandler.hpp"

requestHandler::requestHandler(std::string const request) : _req() {
    std::istringstream iss(request);
    std::string line;
    std::string body;
    bool isFirstLine = true;
    bool isInBody = false;
    while (std::getline(iss, line)) {
        if (isFirstLine) {
            getFirstLine(line);
            isFirstLine = false;
            continue;
        }
        std::string::size_type colonPos = line.find(":");
        std::string key = line.substr(0, colonPos);
        std::string val = line.substr(colonPos + 1);
        _req[key] = val;
        if (line.find("\r\n") && line.size() == 1 && isInBody == false) {
            std::cout << line << std::endl;
            isInBody = true;
            std::getline(iss, line);
        }
        if (isInBody) {
            body += line;
        }
        std::cout << line << std::endl;
    }
    _req["body"] = body;
    std::cout << "dabody:::" << _req["body"] << std::endl;
}


//trouve une maniere plus intelligente de skip les \r\n mec!!!!!!!!

requestHandler::~requestHandler() {}

void requestHandler::getFirstLine(std::string const &line) {
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

std::string requestHandler::handleRequest() {
    std::string path = _req["path"];
    if (isCGI(path) == 0) {
        CGIHandler cgi(_req);
        std::string response = cgi.initCGI();
        return response;
    } else {
        if (_req["method"].compare("GET") != 0)
            return makeErrorResponse(403);
        return makeGetResponse();
    }
}

std::string constructGetResponse(int status, std::ifstream &input) {
    std::string result =  HTTPVER + getStatusCode(status) + "\r\n";
    std::string content((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
    size_t contentLen = content.size();
    result += "Content-Length: " + std::to_string(contentLen) + "\r\n\r\n";
    result += content;
    return result;
}

std::string requestHandler::makeGetResponse() {
    std::ifstream input(SITEPATH + _req["path"]);
    if (!input.is_open()) {
        std::ifstream input(ERRORPATH + intToString(404) + ".html");
        return constructGetResponse(404, input);
    }
    return constructGetResponse(200, input);
}

std::string requestHandler::makeErrorResponse(int err) {
    std::ifstream input(ERRORPATH + intToString(err) + ".html");
    return constructGetResponse(err, input);
}
