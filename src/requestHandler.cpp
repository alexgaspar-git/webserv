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
            std::string val = line.substr(colonPos + 2);
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
    std::string body;
    if (REQBODY.size() > _currentClient->body_size) {
        return buildErrResponse("$#413 Payload Too Large");
    }
    handlePath();
    int ext = getExtension(PATH);
    if (ext == PY || ext == PHP) {
        CGIHandler cgi(_req, ext, _currentClient->cookie);
        body = cgi.initCGI();
    } else {
        body = handleHTML();
    }
    if (isError(body)) {
        return buildErrResponse(body);
    } else {
        return buildResponse(body);
    }
}

std::string requestHandler::buildErrResponse(std::string const &body) {
    std::string errcode = body.substr(2);
    std::string fof = "<html>\n<head>\n    <title>ERROR</title>\n</head>\n<body>\n    <h1 style=\"font-size: 100px; display: flex; justify-content: center; align-items: center; padding: 200px\">" + errcode + "</h1>\n</body>\n</html>";
    std::string response = HTTPVER + errcode + "\r\n";
    response += "Content-Length: " + intToString(fof.size()) + "\r\n";
    response += "\r\n";
    response += fof;
    return response;
}

std::string requestHandler::handleHTML() {
    std::ifstream input(_sitePath + _req["path"]);
    if (!input.is_open()) {
        return "$#404 Not Found";
    }
    std::string body((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
    return body;
}

std::string generatecookiename(std::map<std::string, int> cookie) {
    std::string tmp;
    std::string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    int len = charset.size();
    for (int x = 0; x < 10; x++) {
    	int i = rand() % len;
        tmp += charset[i];
    }
    if (cookie.count(tmp) != 0) {
        tmp = generatecookiename(cookie);
    }
    return (tmp);
}

std::string requestHandler::buildResponse(std::string const &body) {
    std::string response = HTTPVER "200 OK\r\n";
    response += "Content-Length: " + intToString(body.size()) + "\r\n";
    if (_req["Cookie"].empty()) {
        std::string cookieName = generatecookiename(_currentClient->cookie);
        response += "Set-Cookie: " + cookieName + "\r\n";
        _currentClient->cookie[cookieName] = 0;
    }
    response += "\r\n";
    response += body;
    return response;
}