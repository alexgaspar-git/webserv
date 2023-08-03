#include "../includes/requestHandler.hpp"

requestHandler::requestHandler(std::string const request, ConfigParser *pars) : _req(), _currentClient(), _sitePath("www") {
    std::istringstream iss(request);
    std::string line;
    std::string body;
    bool isFirstLine = true;
    bool isInBody = false;
    std::cout << "_______req_______" << std::endl;
    while (std::getline(iss, line)) {
        std::cout << line << std::endl;
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
    std::cout << "_______endreq_______" << std::endl;
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
bool requestHandler::handlePath() {
    const std::string &oldPath = PATH;
    std::string finalPath;

    const std::map<std::string, s_location> &locations = _currentClient->location;
    std::map<std::string, s_location>::const_iterator it;
    for (it = locations.begin(); it != locations.end(); ++it) {
        const std::string &key = it->first;
        const s_location &locationData = it->second;

        if (key == oldPath) {
            finalPath = locationData.root + locationData.index;
        } else if (oldPath.find(key + "/") == 0) {
            finalPath = locationData.root + oldPath.substr(key.length());
        } else if (oldPath.find(key + "?") == 0) {
            finalPath = locationData.root + locationData.index + oldPath.substr(key.length());
        } else {
            continue;
        }

        if (!checkMethod(locationData.method)) {
            return false;
        }
    }

    if (finalPath.empty()) {
        std::map<std::string, s_location>::const_iterator defaultIt = locations.find("/");
        if (defaultIt != locations.end()) {
            const s_location &defaultLocation = defaultIt->second;
            finalPath = defaultLocation.root + ((oldPath.length() == 1) ? defaultLocation.index : oldPath);
        }
    }
    PATH = finalPath;
    std::cout << "final path = " << finalPath << "\n";
    return true;
}

bool requestHandler::checkMethod(const std::string& method) {
    return method.find(MTD) != std::string::npos;
}

int countOccurrences(const std::string& str, char targetChar) {
    int count = 0;
    for (std::size_t i = 0; i < str.length(); ++i) {
        if (str[i] == targetChar)
            count++;
    }
    return count;
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
    if (!handlePath()) {
        return buildErrResponse("$#405 Method Not Allowed");
    }
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
    //rajouter code ici pour error_page
    std::string fof = "<html>\n<head>\n    <title>ERROR</title>\n</head>\n<body>\n    <h1 style=\"font-size: 100px; display: flex; justify-content: center; align-items: center; padding: 200px\">" + errcode + "</h1>\n</body>\n</html>";
    std::string response = HTTPVER + errcode + "\r\n";
    response += "Content-Length: " + intToString(fof.size()) + "\r\n";
    response += "\r\n";
    response += fof;
    return response;
}

std::string requestHandler::handleHTML() {
    std::ifstream input(PATH);
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