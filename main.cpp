#include "Switch.h"
#include "Request.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// generates a random IP
std::string randomIP() {
    return std::to_string(rand() % 256) + "." +
           std::to_string(rand() % 256) + "." +
           std::to_string(rand() % 256) + "." +
           std::to_string(rand() % 256);
}

// generates master queue with sine-wave traffic to simulate realistic bursts and lulls
void generateMasterQueue(std::vector<Request>& masterQueue, int totalServers, int duration) {
    int initialQueueSize = totalServers * 100;  // spec: "usually servers * 100"

    // Phase 1: initial full queue — all arrive at t=1
    for (int i = 0; i < initialQueueSize; i++) {
        int processingTime = 10 + rand() % 50;  // 10–60 ticks
        char type = (rand() % 2 == 0) ? 'P' : 'S';
        masterQueue.push_back(Request(1, processingTime, randomIP(), randomIP(), type));
    }

    // Phase 2: ongoing traffic with sine-wave modulated arrival rate
    //
    // Math: with N servers each taking avg 35 ticks, drain rate ≈ N/35 requests/tick.
    // To trigger scaling, we need the queue to swing across the 50*N and 80*N thresholds.
    // During peaks: arrival rate > drain rate → queue grows → triggers addServer
    // During troughs: arrival rate < drain rate → queue shrinks → triggers removeServer
    //
    // We use a rate that swings between 0 (complete quiet) and ~N/10 (overwhelming)
    // so that peaks strongly outpace servers and troughs let servers fully drain the queue.
    for (int t = 2; t <= duration; t++) {
        // Sine wave oscillates between -1 and 1; we use the RAW sine (not clamped to positive)
        // so that ~half the cycle has zero or near-zero arrivals (quiet period)
        double wave = sin(2.0 * M_PI * t / (duration / 4.0))       // 4 major cycles
                    + 0.5 * sin(2.0 * M_PI * t / (duration / 9.0));  // 9 smaller ripples

        // Map wave to arrival rate:
        // wave ranges from -1.5 to 1.5
        // rate = max(0, wave) * scale  →  arrivals only when wave > 0 (roughly half the time)
        double rate = std::max(0.0, wave) * (totalServers / 8.0);

        // Poisson-approximate: integer part + probabilistic fractional part
        int numRequests = (int)rate;
        if ((rand() % 1000) / 1000.0 < (rate - numRequests)) numRequests++;

        for (int i = 0; i < numRequests; i++) {
            int processingTime = 10 + rand() % 50;
            char type = (rand() % 2 == 0) ? 'P' : 'S';
            masterQueue.push_back(Request(t, processingTime, randomIP(), randomIP(), type));
        }
    }
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
    generateMasterQueue(masterQueue, totalServers, duration);

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