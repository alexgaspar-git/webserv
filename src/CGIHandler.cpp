#include "CGIHandler.hpp"
#include "utils.hpp"

CGIHandler::CGIHandler(std::map<std::string, std::string> const &request, bool type) : _req(request), _body(), _path() {
    if (type == PYTHON) {
        _path = strdup("/usr/bin/python3");
    } else {
        _path = strdup("/usr/bin/php");
    }
    _argv = getArgv();
    _env = getEnv();
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
        path = "." + extractPathString(_req["path"]);
    } else {
        tmp[0] = strdup(php.c_str());
        std::cout << "path: " << _req["path"] << std::endl;
        path = "./www" + extractPathString(_req["path"]);
    }
    tmp[1] = strdup(path.c_str());
    tmp[2] = NULL;

    return tmp;
}

const char **CGIHandler::getEnv() {
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
    tmp.push_back("UPLOAD_DIR=./www/uploads/");
    
    /*
    tmp.push_back("HTTP_COOKIE="); a voir ce que ca fait
    tmp.push_back("SCRIPT_NAME=" + path du script);
    */

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
        close(outPipe[0]);
        close(outPipe[1]);
        close(inPipe[0]);
        close(inPipe[1]);
    } 
    if (pid == 0) {
        dup2(outPipe[1], STDOUT_FILENO);
        dup2(inPipe[0], STDIN_FILENO);
        close(inPipe[0]);
        close(inPipe[1]);
        close(outPipe[0]);
        close(outPipe[1]);
        execve(_path, (char* const*)_argv, (char* const*)_env);
    } else {
        close(outPipe[1]);
        close(inPipe[0]);
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

std::string CGIHandler::constructResponse() {
    std::string response = HTTPVER + getStatusCode(200) + "\r\n";
    size_t contentLen = _body.size();
    response += "Content-Length: " + std::to_string(contentLen) + "\r\n\r\n";
    response += _body;
    return response;
}

std::string CGIHandler::initCGI() {

    if (!execCGI()) {
        return "";
    }

    std::string response = constructResponse();

    std::cout << response << std::endl;

    return response;

}

std::string extractBoundary(std::string &line) {
    std::string const boundaryStr = "boundary=";
    std::string::size_type boundPos = line.find_last_of('-');
    if (boundPos == std::string::npos) {
        std::cout << "??" << std::endl;
        return "";
    };
    std::string ret = line.substr(boundPos + 1);
    return ret;
}
