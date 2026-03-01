/**
 * @file WebServer.h
 * @brief Defines the WebServer class that processes individual requests.
 */

#ifndef WEBSERVER_H
#define WEBSERVER_H

#include "Request.h"

/**
 * @class WebServer
 * @brief Simulates a web server that processes one request at a time.
 *
 * A WebServer is either available (idle) or busy processing a request.
 * Each clock tick decrements the remaining processing time of the current request.
 */
class WebServer {
    private:
        Request currentRequest;  ///< The request currently being processed
        int timeRemaining;       ///< Clock cycles remaining for the current request
        bool isAvailable;        ///< Whether the server is available to take a new request

    public:
        /**
         * @brief Default constructor. Creates an available (idle) server.
         */
        WebServer();

        /**
         * @brief Checks if the server is available.
         * @return True if the server is idle, false if busy.
         */
        bool getAvailability() const;

        /**
         * @brief Gets the remaining processing time for the current request.
         * @return Number of clock cycles remaining.
         */
        int getTimeRemaining() const;

        /**
         * @brief Assigns a request to this server for processing.
         * @param req The request to process.
         */
        void assignRequest(const Request req);

        /**
         * @brief Advances the server by one clock cycle.
         * @return True if a request was completed this tick, false otherwise.
         */
        bool tick();
};

#endif