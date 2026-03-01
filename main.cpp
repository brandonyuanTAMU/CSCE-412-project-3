#include "Switch.h"
#include "Request.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>

// generates a random IP
std::string randomIP() {
    return std::to_string(rand() % 256) + "." +
           std::to_string(rand() % 256) + "." +
           std::to_string(rand() % 256) + "." +
           std::to_string(rand() % 256);
}

// generates a random request
Request randomRequest(int currentTime, int duration) {
    std::string ipIn = randomIP();
    std::string ipOut = randomIP();
    int arrivalTime = currentTime + rand() % duration;
    int processingTime = 1 + rand() % 10;
    char type = (rand() % 2 == 0) ? 'P' : 'S';
    return Request(arrivalTime, processingTime, ipIn, ipOut, type);
}

int main() {
    srand(time(nullptr));
    std::ifstream config("input.txt");
    if (!config.is_open()) {
        std::cerr << "Could not open input.txt\n";
        return 1;
    }

    int serversP = 5, serversS = 5, duration = 10000, cooldown = 100;
    std::string logfile = "log.txt";
    std::vector<std::string> blockedIPs;

    std::string line;
    while (std::getline(config, line)) {
        std::istringstream ss(line);
        std::string key, value;
        if (std::getline(ss, key, '=') && std::getline(ss, value)) {
            if (key == "servers_p")   serversP  = std::stoi(value);
            else if (key == "servers_s")   serversS  = std::stoi(value);
            else if (key == "duration")    duration  = std::stoi(value);
            else if (key == "cooldown")    cooldown  = std::stoi(value);
            else if (key == "logfile")     logfile   = value;
            else if (key == "blocked") {
                std::istringstream ips(value);
                std::string ip;
                while (std::getline(ips, ip, ',')) {
                    blockedIPs.push_back(ip);
                }
            }
        }
    }

    // switch obj for sorting req types
    Switch sw(serversP, serversS, cooldown, logfile, blockedIPs);

    // "master" q contains all requests, will send to LB based on time
    int totalServers = serversP + serversS;
    std::vector<Request> masterQueue;
    masterQueue.reserve(totalServers * 100);
    for (int i = 0; i < totalServers * 100; i++) {
        masterQueue.push_back(randomRequest(0, duration));
    }

    // main loop
    for (int t = 1; t <= duration; t++) {
        for (const Request& req : masterQueue) {
            if (req.getArrivalTime() == t) {
                sw.addRequest(req);
            }
        }
        sw.tick();
    }
    sw.end();
    return 0;
}