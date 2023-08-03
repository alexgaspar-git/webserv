#include "CGIHandler.hpp"
#include "utils.hpp"

CGIHandler::CGIHandler(std::map<std::string, std::string> const &request, int type, std::map<std::string, int> &cookie) : _req(request), _body(), _path() {
    if (type == PY) {
        _path = strdup("/usr/bin/python3");
    } else {
        _path = strdup("/usr/bin/php");
    }
    _argv = getArgv();
    _env = getEnv(cookie);
};

CGIHandler::~CGIHandler() {
    delete[] _argv;
    delete[] _env;
}

const char** CGIHandler::getArgv() {
    std::string py = "python3";
    std::string php = "php";
    std::string path;

    const char** tmp = new const char*[3];
    if (strcmp(_path, "/usr/bin/python3") == 0) {
        tmp[0] = strdup(py.c_str());
    } else {
        tmp[0] = strdup(php.c_str());
    }
    path = extractPathString(_req["path"]);
    tmp[1] = strdup(path.c_str());
    tmp[2] = NULL;

    return tmp;
}

const char **CGIHandler::getEnv(std::map<std::string, int> &cookie) {
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
    tmp.push_back("UPLOAD_DIR=./www/images");

    if (_req["Cookie"].size() == 0) {
        tmp.push_back("NUMBER=0");
    } else {
        tmp.push_back("NUMBER=" + intToString(cookie[_req["Cookie"]]));
        if (PATH.find("cookie") != std::string::npos)
            cookie[_req["Cookie"]]++;
    }

    const char** env = new const char*[tmp.size() + 1];
    
    for (size_t i = 0; i < tmp.size(); i++) {
        env[i] = strdup(tmp[i].c_str());
        std::cout << env[i] << std::endl;
    }
    
    env[tmp.size()] = NULL;
    return env;
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
        dup2(outPipe[1], STDOUT_FILENO);
        dup2(inPipe[0], STDIN_FILENO);
        closer(outPipe[0], outPipe[1], inPipe[0], inPipe[1]);
        execve(_path, (char* const*)_argv, (char* const*)_env);
    } else {
        close(outPipe[1]);
        close(inPipe[0]);
        std::cout << "____body____" << std::endl;
        std::cout << _req["body"] << std::endl;
        std::cout << "_____endbody_______" << std::endl;
        if (write(inPipe[1], _req["body"].c_str(), _req["body"].size()) == -1) {
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
    return true;
}

std::string CGIHandler::initCGI() {
    if (!execCGI()) {
        return "$#500 Internal Server Error";
    };
    return _body;
}
