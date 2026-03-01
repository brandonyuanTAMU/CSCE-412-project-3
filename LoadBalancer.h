#ifndef LOADBALANCER_H
#define LOADBALANCER_H

#include "Request.h"
#include "WebServer.h"

#include <vector>
#include <queue>
#include <string>

class LoadBalancer {
    private:
        std::string name;
        std::queue<Request> requests;
        std::vector<WebServer> servers;
        int time;
        int lastAdjustedTime;
        int adjustCooldown;
        std::vector<std::string> blockedIPs;
        std::string logfile;

        void addServer();
        void removeServer();
        void assignRequests();

        void logEvent(const std::string& event) const;
    public:
        LoadBalancer(int initialservers, int cooldown, const std::string& logfile, const std::vector<std::string>& blockedips, const std::string& name);
        void tick();
        void addRequest(const Request& req);
        void end();
};

#endif