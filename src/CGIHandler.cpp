#include "CGIHandler.hpp"

CGIHandler::CGIHandler(std::map<std::string, std::string> const &request) : _req(request), _body(), _path("/usr/bin/python3") {
    _argv = getArgv();
    _env = getEnv();
};

CGIHandler::~CGIHandler() {
    delete[] _argv;
    delete[] _env;
}

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
    tmp.push_back("HTTP_USER_AGENT=" + _req["User-Agent"]);
    tmp.push_back("REQUEST_METHOD=" + _req["method"]);
    tmp.push_back("SERVER_NAME=" + _req["Host"]);
    tmp.push_back("SERVER_HOST=" + _req["port"]);
    tmp.push_back("SERVER_PROTOCOL=" HTTPVER);
    tmp.push_back("HTTP_ACCEPT_LANGUAGE=" + _req["Accept"]);
    /*
    tmp.push_back("CONTENT_LENGTH=" + sizedebody)
    tmp.push_back("HTTP_COOKIE="); a voir ce que ca fait
    tmp.push_back("SCRIPT_NAME=" + path du script);
    */

    const char** env = new const char*[tmp.size() + 1];
    
    for (size_t i = 0; i < tmp.size(); i++) {
        env[i] = strdup(tmp[i].c_str());
    }
    
    env[tmp.size()] = NULL;
    return env;
}

bool CGIHandler::execCGI() {
    int pfd[2];
    if (pipe(pfd) == -1) {
        return false;
    }
    int pid = fork();
    if (pid == -1) {
        close(pfd[0]);
        close(pfd[1]);
    } 
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
    // ne gere que GET pour l'instant
    std::string method = _req["method"];
    std::string response;

    if (!execCGI()) {
        return "";
    }

    response = constructResponse();

    return response;

}

// IL FO EUU CONSTRUCT LA REPONSE ET ENSUITE BIEN FAIRE UNE MAIN FONCTION QUI GERE TOUT
// merci alex du passé

//nouvelle idée pour erreurs, passer en string un identifiant d'érreur en mode "#!404" et avant d'envoyer la reponse, parser si c'est une erreur ou si c'est une reponse typique


