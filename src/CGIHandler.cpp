#include "CGIHandler.hpp"
#include "utils.hpp"

CGIHandler::CGIHandler(std::map<std::string, std::string> const &request, int type, std::map<std::string, int> &cookie) : _req(request), _body(), _path(), _cookie(cookie) {
    if (type == PY) {
        _path = "/usr/bin/python3";
    } else {
        _path = "/usr/bin/php";
    }
};

CGIHandler::~CGIHandler() {}

void CGIHandler::getEnv(std::map<std::string, int> &cookie) {
    std::vector<std::string> tmp;

    if (_req["method"] == "GET") {
        tmp.push_back("QUERY_STRING=" + extractQueryString(_req["path"]));
    } else if (_req["method"] == "POST") {
        tmp.push_back("QUERY_STRING=");
        
    }
    tmp.push_back("HTTP_USER_AGENT=" + _req["User-Agent"]);
    tmp.push_back("REQUEST_METHOD=" + _req["method"]);
    tmp.push_back("SERVER_NAME=" + _req["Host"]);
    tmp.push_back("SERVER_HOST=" + _req["port"]);
    tmp.push_back("SERVER_PROTOCOL=" HTTPVER);
    tmp.push_back("HTTP_ACCEPT_LANGUAGE=" + _req["Accept"]);
    tmp.push_back("CONTENT_LENGTH=" + intToString(_req["body"].size()));
    tmp.push_back("CONTENT_TYPE=" + _req["Content-Type"]);
    tmp.push_back("UPLOAD_DIR=./www/images/");
    tmp.push_back("INDEXPATH=" + _req["autoindex"]);

    if (_req["Cookie"].size() == 0) {
        tmp.push_back("NUMBER=0");
        std::cout << "i rentre ici" << std::endl;
    } else {
        tmp.push_back("NUMBER=" + intToString(cookie[_req["Cookie"]]));
    }
    _env = new char*[tmp.size() + 1];
    for (size_t i = 0; i < tmp.size(); i++) {
        _env[i] = new char[(tmp[i].size()) + 1];
        std::strcpy(_env[i], tmp[i].c_str());
    }
    _env[tmp.size()] = NULL;
}

void stopCGISig() {
    std::signal(SIGINT, SIG_IGN);
}

void restoreCGISig() {
    std::signal(SIGINT, SIG_DFL);
}

bool CGIHandler::execCGI() {
    int outPipe[2];
    int inPipe[2];
    if (pipe(outPipe) == -1 || pipe(inPipe) == -1) {
        return false;
    }
    int pid = fork();
    if (pid == -1) {
        closer(outPipe[0], outPipe[1], inPipe[0], inPipe[1]);
    } 
    if (pid == 0) {
        getEnv(_cookie);

        if (dup2(outPipe[1], STDOUT_FILENO) == -1)
            exit(1);
        if (dup2(inPipe[0], STDIN_FILENO) == -1)
            exit(1);
        closer(outPipe[0], outPipe[1], inPipe[0], inPipe[1]);

        std::string py = "python3";
        std::string php = "php";

        char *tmp[3];

        if (_path.compare("/usr/bin/python3") == 0) {
            tmp[0] = (char *)py.c_str();
        } else {
            tmp[0] = (char *)php.c_str();
        }
        std::string path = extractPathString(_req["path"]);
        tmp[1] = (char *)path.c_str();
        tmp[2] = NULL;

        execve(_path.c_str(), tmp, _env);
        exit(1);
    } else {
        stopCGISig();
        close(outPipe[1]);
        close(inPipe[0]);
        if (write(inPipe[1], _req["body"].c_str(), _req["body"].size()) <= 0) {
            return false;
        }
        close(inPipe[1]);
        char buffer[512];
        size_t bytesRead;
        while ((bytesRead = read(outPipe[0], buffer, sizeof(buffer))) > 0) {
            _body.append(buffer, bytesRead);
        }
        close(outPipe[0]);
    }
    waitpid(pid, NULL, 0);
    restoreCGISig();
    return true;
}

std::string CGIHandler::initCGI() {
    if (PATH.find("cookie") != std::string::npos)
        _cookie[_req["Cookie"]]++;
    if (!execCGI() || _body.empty()) {
        return "$#500 Internal Server Error";
    };
    return _body;
}
