#include "LoadBalancer.h"
#include "Request.h"
#include "WebServer.h"

#include <iostream>
#include <fstream>

LoadBalancer::LoadBalancer(int initservers, int cooldown, const std::string& logfile, const std::vector<std::string>& blockedips)
    : time(0), lastAdjustedTime(0), adjustCooldown(cooldown), logfile(logfile), blockedIPs(blockedips) {
    for (int i=0; i<initservers; ++i) {
        servers.push_back(WebServer());
    }
    logEvent("LoadBalancer initialized with num servers: " + std::to_string(initservers));
}

// private functions
void LoadBalancer::logEvent(const std::string& event) const {
    std::cout << "[t=" << time << "] " << event << "\n";

    std::ofstream file(logfile, std::ios::app);
    if (file.is_open()) {
        file << "[t=" << time << "] " << event << "\n";
    }
}

void LoadBalancer::addServer() {
    servers.push_back(WebServer());
    logEvent("Server added, total servers: " + std::to_string(servers.size()));
}

void LoadBalancer::removeServer() {
    for (size_t i = 0; i < servers.size(); i++) {
        if (servers[i].getAvailability()) {
            servers.erase(servers.begin() + i);
            logEvent("Server removed, total servers: " + std::to_string(servers.size()));
            return;
        }
    }
    logEvent("Couldn't remove a server (all busy)");
}

void LoadBalancer::assignRequests() {
    for (WebServer& server : servers) {
        if (server.getAvailability() && !requests.empty()) {
            server.assignRequest(requests.front());
            requests.pop();
        }
    }
}

// public functions
void LoadBalancer::addRequest(const Request& req) {
    for (const std::string& blocked : blockedIPs) {
        if (req.getIpIn().rfind(blocked, 0) == 0) { // check for prefix
            logEvent("Blocked request from IP: " + req.getIpIn());
            return;
        }
    }
    requests.push(req);
}

void LoadBalancer::tick() {
    ++time;
    for (WebServer& server : servers) {
        if (!server.getAvailability()) {
            bool finished = server.tick();
            if (finished) {
                logEvent("Request completed at time " + std::to_string(time));
            }
        }
    }
    assignRequests();
    if (time - lastAdjustedTime >= adjustCooldown) {
        if (requests.size() > 80 * servers.size()) {
            addServer();
            lastAdjustedTime = time;
        } else if (requests.size() < 50 * servers.size() && servers.size() > 1) {
            removeServer();
            lastAdjustedTime = time;
        }
    }
}

void LoadBalancer::end() {
    logEvent("Simulation ended at time: " + std::to_string(time));
    logEvent("Final server count: " + std::to_string(servers.size()));
    logEvent("Remaining queue size: " + std::to_string(requests.size()));
}