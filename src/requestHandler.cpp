#include "../includes/requestHandler.hpp"

requestHandler::requestHandler(std::string const request, ConfigParser *pars) : _req(), _currentClient(), _sitePath("www") {
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
        if (!isInBody) {
            std::string::size_type colonPos = line.find(":");
            std::string key = line.substr(0, colonPos);
            std::string val = line.substr(colonPos + 1);
            _req[key] = cleanLine(val);
        }
        if (line.find("\r\n") && line.size() == 1 && isInBody == false) {
            isInBody = true;
            std::getline(iss, line);
        }
        if (isInBody) {
            body += line;
            body += "\n";
        }
    }
    _currentClient = getCurrentClient(pars);
    _req["body"] = body;
}

requestHandler::~requestHandler() {}

void requestHandler::getFirstLine(std::string const &line) {
    std::string keys[3] = {"method", "path", "version"};
    std::istringstream iss(line);
    std::string word;
    int i = 0;
    while (iss >> word) {
        _req[keys[i++]] = word;
    }
}

void requestHandler::handlePath() {
    std::map<std::string, s_location>::iterator it;
    for (it = _currentClient->location.begin(); it != _currentClient->location.end(); it++) {
        std::string key = it->first;
        if (_req["path"] == key || _req["path"] + "/" == key) {
            _sitePath = _currentClient->location[key].root;
            if (_req["path"] == key)
                _sitePath += "/";
            _req["path"] = _currentClient->location[key].index;
        }
    }
}

std::map<std::string, std::string> requestHandler::getMap() { 
    return _req;
}

std::vector<s_conf>::iterator requestHandler::getCurrentClient(ConfigParser *pars) {
    std::vector<s_conf>::iterator it;
    for (it = pars->_config->begin(); it != pars->_config->end(); it++) {
        if (_req["Host"].find(it->port) != std::string::npos) {
            return it;
        }
    }
    return it;
}

std::string requestHandler::handleRequest() {
    handlePath();
    std::string path = _req["path"];
    std::string response;
    int ext = getExtension(path);

    if (_req["body"].size() > _currentClient->body_size) {
        return "#!413";
    }

    if (ext == PY || ext == PHP) {
        CGIHandler cgi(_req, ext, _currentClient->cookie);
        response = cgi.initCGI();
    } else {
        response = makeGetResponse();
    }
    return response;
}

std::string requestHandler::constructGetResponse(int status, std::ifstream &input) {
    std::string result =  HTTPVER + getStatusCode(status) + "\r\n";
    std::string content((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
    size_t contentLen = content.size();
    result += "Content-Length: " + std::to_string(contentLen) + "\r\n";
    if (_req["Cookie"].empty()) {
        result += "Set-Cookie: cookieName\r\n";
        _currentClient->cookie["cookieName"] = 1;
    }
    result += "\r\n";
    result += content;
    return result;
}

std::string requestHandler::makeGetResponse() {
    std::ifstream input(_sitePath + _req["path"]);
    if (!input.is_open()) {
        std::ifstream inputErr(ERRORPATH + intToString(404) + ".html");
        return constructGetResponse(404, inputErr);
    }
    return constructGetResponse(200, input);
}

std::string requestHandler::makeErrorResponse(int err) {
    std::ifstream input(ERRORPATH + intToString(err) + ".html");
    return constructGetResponse(err, input);
}
