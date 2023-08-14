#include "../includes/requestHandler.hpp"

requestHandler::requestHandler(std::string const request, ConfigParser *pars, int port) : _req(), _currentClient(), _sitePath("www"), _isForbidden(false), _isAutoIndex(false), _noRoot(false), _badRequest(false), _isHTTPS(false), _redirectCode(""), _redirectLink(""), _noCurrentClient(false) {
    std::istringstream iss(request);
    std::string line;
    std::string body;
    bool isFirstLine = true;
    bool isInBody = false;
    while (std::getline(iss, line)) {
        if (isFirstLine) {
            if (!getFirstLine(line)) {
                return ;
            }
            isFirstLine = false;
            continue;
        }
        if (!isInBody) {
            std::string::size_type colonPos = line.find(":");
            if (colonPos != std::string::npos) {
                std::string key = line.substr(0, colonPos);
                std::string val = line.substr(colonPos + 2);
                _req[key] = cleanLine(val);
            }
        }
        if (line.find("\r\n") && line.size() == 1 && isInBody == false) {
            isInBody = true;
            std::getline(iss, line);
        }
        if (isInBody) {
            body += line;
            body += "\n";
        }
        _req["all"] += line;
    }
    _currentClient = getCurrentClient(pars, port);
    _req["body"] = body;
}

requestHandler::~requestHandler() {}

bool requestHandler::getFirstLine(std::string const &line) {
    std::string keys[3] = {"method", "path", "version"};
    std::istringstream iss(line);
    if (line.find("HTTP") == std::string::npos) {
        _isHTTPS = true;
        return false;
    }
    std::string word;
    int i = 0;
    while (iss >> word) {
        _req[keys[i++]] = word;
    }
    if (_req["path"].find("//") != std::string::npos)
        _badRequest = true;
    return true;
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
        if (!checkMethod(locationData, finalPath))
            return false;
    }

    if (finalPath.empty()) {
        std::map<std::string, s_location>::const_iterator defaultIt = locations.find("/");
        if (defaultIt != locations.end()) {
            const s_location &defaultLocation = defaultIt->second;
            if (defaultLocation.index.empty()) {
                if (defaultLocation.autoindex == "on") {
                    _isAutoIndex = true;
                } else {
                    _isForbidden = true;
                }
            } else {
                finalPath = defaultLocation.root + ((oldPath.length() == 1) ? defaultLocation.index : oldPath);
            }
            if (getExtension(finalPath) != HTML) {
                if (defaultLocation.autoindex == "on") {
                    _isAutoIndex = true;
                } else {
                    _isForbidden = true;
                }
            } 
        }
    }
    PATH = finalPath;
    return true;
}

bool requestHandler::checkMethod(const s_location &locationData, std::string finalPath) {
    if (finalPath == "./www/images" && locationData.autoindex == "off") {
        _isForbidden = true;
    } else if (locationData.index == "/delete.py"
        && locationData.method.find("DELETE") == std::string::npos)
        return false;
    if (!locationData.redirect.empty()) {
        _redirectLink = locationData.redirect.begin()->second;
        _redirectCode = locationData.redirect.begin()->first;
        return true;
    } else if (locationData.root.empty()) {
        _noRoot = true;
    } else if (locationData.index.empty() && locationData.autoindex == "off") {
        if (locationData.root != "./www/images") {
            _isForbidden = true;
        }
    } else if (locationData.index.empty() && locationData.autoindex == "on") {
        _isAutoIndex = true;
    }
    return locationData.method.find(MTD) != std::string::npos;
}

int countOccurrences(const std::string& str, char targetChar) {
    int count = 0;
    for (std::size_t i = 0; i < str.length(); ++i) {
        if (str[i] == targetChar)
            count++;
    }
    return count;
}

std::vector<s_conf>::iterator requestHandler::getCurrentClient(ConfigParser *pars, int port) {
    std::vector<s_conf>::iterator it;
    for (it = pars->_config->begin(); it != pars->_config->end(); it++) {
        if (intToString(port).find(it->port) != std::string::npos
        && (_req["Host"].find(it->name + ":" + it->port) == 0
        || _req["Host"].find("localhost:" + it->port) == 0)) {
            return it;
        }
    }
    std::cerr << "Client error." << std::endl;
    _noCurrentClient = true;
    return it;
}

std::string requestHandler::handleRequest() {
    if (_isHTTPS)
        return "https";
    if (_badRequest)
        return buildErrResponse("$#400 Bad Request");
    if (REQBODY.size() > _currentClient->body_size)
        return buildErrResponse("$#413 Payload Too Large");
    if (!handlePath())
        return buildErrResponse("$#405 Method Not Allowed");
    if (_redirectCode != "")
        return buildRedirResponse();
    if (_isForbidden)
        return buildErrResponse("$#403 Forbidden");
    if (_noRoot)
        return buildErrResponse("$#500 Internal Server Error");
    int ext = getExtension(PATH);
    if (_isAutoIndex) {
        if (ext == OTHER) {
            _req["autoindex"] = PATH;
            PATH = "./www/php/autoindex.php";
        }
    }
    std::string body;
    ext = getExtension(PATH);
    if (ext == PY || ext == PHP) {
        CGIHandler cgi(_req, ext, _currentClient->cookie);
        body = cgi.initCGI();
    } else {
        body = handleHTML(PATH);
    }
    if (isError(body)) {
        return buildErrResponse(body);
    } else {
        return buildResponse(body);
    }
}

std::string const requestHandler::buildRedirResponse() {
    std::string response = HTTPVER + _redirectCode + " MOVED PERMANENTLY\r\n";
    response += "Location: " + _redirectLink;
    return response;
}

bool requestHandler::defaultError(std::string const &errcode) {
    return !_currentClient->location["/"].error[errcode].empty();
}


std::string requestHandler::buildErrResponse(std::string const &body) {
    std::string errmsg = body.substr(2);
    std::string errcode = errmsg.substr(0, 3);
    std::string response = HTTPVER + errmsg + "\r\n";
    if (!defaultError(errcode)) {
        std::string fof = "<html>\n<head>\n    <title>ERROR</title>\n</head>\n<body>\n    <h1 style=\"font-size: 100px; display: flex; justify-content: center; align-items: center; padding: 200px\">" + errmsg + "</h1>\n</body>\n</html>";
        response += "Content-Length: " + intToString(fof.size()) + "\r\n";
        response += "\r\n";
        response += fof;
    } else {
        std::string errPath = _currentClient->location["/"].error[errcode];
        std::string errBody = handleHTML(errPath);
        if (isError(errBody)) {
            _currentClient->location["/"].error[errcode] = "";
            return buildErrResponse(errBody);
        }
        response += "Content-Length: " + intToString(errBody.size()) + "\r\n";
        response += "\r\n";
        response += errBody;
    }
    return response;
}

std::string requestHandler::handleHTML(std::string const &path) {
    std::ifstream input(path);
    if (!input.is_open()) {
        return "$#404 Not Found";
    }
    std::string body((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
    input.close();
    return body;
}

std::string requestHandler::buildResponse(std::string const &body) {
    std::string response = HTTPVER "200 OK\r\n";
    response += "Content-Length: " + intToString(body.size()) + "\r\n";
    if (_req["Cookie"].empty()) {
        std::string cookieName = generateCookieName(_currentClient->cookie);
        response += "Set-Cookie: " + cookieName + "\r\n";
        _currentClient->cookie[cookieName] = 0;
    }
    response += "\r\n";
    response += body;
    return response;
}