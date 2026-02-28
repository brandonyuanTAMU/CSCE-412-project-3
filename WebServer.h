#ifndef WEBSERVER_H
#define WEBSERVER_H

#include "Request.h"

class WebServer {
    private:
        Request* currentRequest;
        int timeRemaining;
        bool isAvailable;
    public:
        WebServer();
        bool getAvailability() const;
        int getTimeRemaining() const;
        void assignRequest(Request* req);
        void tick();
};

#endif