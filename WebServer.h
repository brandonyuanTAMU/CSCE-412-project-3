#ifndef WEBSERVER_H
#define WEBSERVER_H

#include "Request.h"

class WebServer {
    private:
        Request* currentRequest;
        int timeRemaining;
        bool isAvailable;
    public:
        WebServer() : currentRequest(nullptr), timeRemaining(0), isAvailable(true) {}
        bool getAvailability() const { return isAvailable; }
        int getTimeRemaining() const { return timeRemaining; }
        void assignRequest(Request* req) {
            currentRequest = req;
            isAvailable = False;
            timeRemaining = currentRequest->getProcessingTime;
        }
        void tick() {
            if (!isAvailable) {
                --timeRemaining;
                if (timeRemaining == 0) {
                    isAvailable = True;
                    currentRequest = nullptr;
                }
            }
        }
};

#endif