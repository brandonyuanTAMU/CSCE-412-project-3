#include "WebServer.h"

WebServer::WebServer() : currentRequest(), timeRemaining(0), isAvailable(true) {}

bool WebServer::getAvailability() const {
    return isAvailable;
}

int WebServer::getTimeRemaining() const {
    return timeRemaining;
}

void WebServer::assignRequest(const Request req) {
    currentRequest = req;
    isAvailable = false;
    timeRemaining = req.getProcessingTime();
}

bool WebServer::tick() {
    if (!isAvailable) {
        --timeRemaining;

        if (timeRemaining <= 0) {
            isAvailable = true;
            currentRequest = Request();
            return true;
        }
    }
    return false;
}