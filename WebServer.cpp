/**
 * @file WebServer.cpp
 * @brief Implementation of the WebServer class.
 */

#include "WebServer.h"

/**
 * @brief Default constructor. Initializes an idle server with no active request.
 */
WebServer::WebServer() : currentRequest(), timeRemaining(0), isAvailable(true) {}

/**
 * @brief Checks if the server is currently available.
 * @return True if the server has no active request.
 */
bool WebServer::getAvailability() const {
    return isAvailable;
}

/**
 * @brief Gets the remaining processing time for the current request.
 * @return Clock cycles remaining, or 0 if idle.
 */
int WebServer::getTimeRemaining() const {
    return timeRemaining;
}

/**
 * @brief Assigns a new request to this server.
 *
 * Marks the server as busy and sets the processing countdown
 * to the request's processing time.
 *
 * @param req The request to be processed.
 */
void WebServer::assignRequest(const Request req) {
    currentRequest = req;
    isAvailable = false;
    timeRemaining = req.getProcessingTime();
}

/**
 * @brief Advances the server by one clock cycle.
 *
 * If the server is busy, decrements the remaining time. When the
 * remaining time reaches zero, the server becomes available again.
 *
 * @return True if a request was completed during this tick.
 */
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