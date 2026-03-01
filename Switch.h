#ifndef SWITCH_H
#define SWITCH_H

#include "LoadBalancer.h"
#include "Request.h"
#include <string>
#include <vector>

class Switch {
    private:
        LoadBalancer processingLB;
        LoadBalancer streamingLB;

    public:
        Switch(int pServers, int sServers, int cooldown,
               const std::string& logfile,
               const std::vector<std::string>& blockedIPs);

        void addRequest(const Request& req);
        void tick();
        void end();
};

#endif