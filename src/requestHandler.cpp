#include "../includes/requestHandler.hpp"

requestHandler::requestHandler(std::string const request) : _req() {
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

std::string requestHandler::handleRequest() {
    std::string path = _req["path"];
    if (isCGI(path) == 0) {
        CGIHandler jeff(_req);
        return jeff.execCGI();;
    } else {
        if (_req["method"].compare("GET") != 0)
            return makeErrorResponse(403);
        return makeGetResponse();
    }
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

std::string constructGetResponse(int status, std::ifstream &input) {
    std::string result =  HTTPVER + getStatusCode(status) + "\r\n";
    std::string content((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
    size_t contentLen = content.size();
    result += "Content-Length: " + std::to_string(contentLen) + "\r\n\r\n";
    result += content;
    return result;
}

std::string intToString(int value) {
    std::stringstream ss;
    ss << value;
    return ss.str();
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
