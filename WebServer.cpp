#include "WebServer.h"

WebServer::WebServer() : currentRequest(nullptr), timeRemaining(0), isAvailable(true) {}

bool WebServer::getAvailability() const {
    return isAvailable;
}

int WebServer::getTimeRemaining() const {
    return timeRemaining;
}

void WebServer::assignRequest(Request* req) {
    currentRequest = req;
    isAvailable = false;
    timeRemaining = req->getProcessingTime();
}

void WebServer::tick() {
    if (!isAvailable) {
        --timeRemaining;

        if (timeRemaining <= 0) {
            isAvailable = true;
            currentRequest = nullptr;
        }
    }
}