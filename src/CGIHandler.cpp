#include "CGIHandler.hpp"

CGIHandler::CGIHandler(std::map<std::string, std::string> const &request) : _req(request), _body(), _path("/usr/bin/python3") {
    _argv = getArgv();
    _env = getEnv();
};

// utils

std::string extractPathString(const std::string& inputString) {
    size_t queryEnd = inputString.find('?');
    return inputString.substr(0, queryEnd);
}

std::string extractQueryString(const std::string& inputString) {
    size_t queryStart = inputString.find('?');
    if (queryStart != std::string::npos) {
        return inputString.substr(queryStart + 1);
    }
    return "";
}

//

const char** CGIHandler::getArgv() {
    std::string py = "python3";
    std::string path = "." + extractPathString(_req["path"]);

    const char** tmp = new const char*[3];
    tmp[0] = strdup(py.c_str());
    tmp[1] = strdup(path.c_str());
    tmp[2] = NULL;

    return tmp;
}

const char **CGIHandler::getEnv() {
    std::vector<std::string> tmp;

    tmp.push_back("QUERY_STRING=" + extractQueryString(_req["path"]));

    const char** env = new const char*[tmp.size() + 1];
    
    for (size_t i = 0; i < tmp.size(); i++) {
        env[i] = strdup(tmp[i].c_str());
    }
    
    env[tmp.size()] = NULL;
    return env;
}

std::string CGIHandler::execCGI() {
    int pfd[2];
    if (pipe(pfd) == -1) {
        //gerer er0r
    }
    int pid = fork();
    if (pid == -1) {
        //gerer er0r
    } 
    //child
    if (pid == 0) {
        dup2(pfd[1], STDOUT_FILENO);
        close(pfd[0]);
        close(pfd[1]);
        execve(_path, (char* const*)_argv, (char* const*)_env);
    } else {
        close(pfd[1]);
        char buffer[512];
        size_t bytesRead;
        while ((bytesRead = read(pfd[0], buffer, sizeof(buffer))) > 0) {
            _body.append(buffer, bytesRead);
        }
        close(pfd[0]);
    }
    waitpid(pid, NULL, 0);

    std::string str =  HTTPVER + getStatusCode(200) + "\r\n";
    size_t contentLen = _body.size();
    str += "Content-Length: " + std::to_string(contentLen) + "\r\n\r\n";
    str += _body;
    return str;
}

// std::string initCGI()

// IL FO EUU CONSTRUCT LA REPONSE ET ENSUITE BIEN FAIRE UNE MAIN FONCTION QUI GERE TOUT
// merci alex du passé

