/**
 * @file main.cpp
 * @brief Entry point for the Load Balancer Simulation.
 *
 * Reads configuration from input.txt, generates a master queue of requests
 * using sine-wave modulated traffic patterns, and runs the simulation
 * through a Switch that routes requests to type-specific load balancers.
 *
 * @section config Configuration File Format (input.txt)
 * @code
 * servers_p=5
 * servers_s=5
 * duration=10000
 * cooldown=100
 * logfile=log.txt
 * blocked=192.168.1.,10.0.0.
 * @endcode
 */

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
#define M_PI 3.14159265358979323846  ///< Pi constant (fallback if not defined by cmath)
#endif

/**
 * @brief Generates a random IPv4 address string.
 * @return A string in the format "X.X.X.X" where each octet is 0-255.
 */
std::string randomIP() {
    return std::to_string(rand() % 256) + "." +
           std::to_string(rand() % 256) + "." +
           std::to_string(rand() % 256) + "." +
           std::to_string(rand() % 256);
}

/**
 * @brief Generates the master queue of requests with sine-wave traffic patterns.
 *
 * The queue is built in two phases:
 * - **Phase 1**: An initial "full queue" of `totalServers * 100` requests at t=1.
 * - **Phase 2**: Ongoing traffic modulated by overlapping sine waves, creating
 *   4 major burst cycles and 9 smaller ripples. Arrivals only occur when the
 *   sine wave is positive (~half the time), allowing the queue to drain during
 *   quiet periods and triggering dynamic server scaling.
 *
 * @param masterQueue Reference to the vector to populate with requests.
 * @param totalServers Total number of servers across all load balancers.
 * @param duration Total simulation duration in clock cycles.
 */
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

/**
 * @brief Main entry point for the load balancer simulation.
 *
 * Reads configuration from input.txt, creates a Switch with Processing
 * and Streaming load balancers, generates the master request queue,
 * and runs the simulation loop for the configured duration.
 *
 * @return 0 on success, 1 if configuration file cannot be opened.
 */
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

    // Parse configuration file (key=value format)
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

    // Create switch with Processing and Streaming load balancers
    Switch sw(serversP, serversS, cooldown, logfile, blockedIPs);

    // Generate master queue of all requests with arrival times
    int totalServers = serversP + serversS;
    std::vector<Request> masterQueue;
    generateMasterQueue(masterQueue, totalServers, duration);

    // Main simulation loop: inject requests at their arrival times and tick
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