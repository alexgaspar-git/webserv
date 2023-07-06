#include "requestHandler.hpp"

requestHandler::requestHandler(std::string const request) {
    std::istringstream iss(request);
    std::string line;
    while (std::getline(iss, line)) {
        std::cout << "line = " << line << std::endl;
    }
}

requestHandler::~requestHandler() {

}