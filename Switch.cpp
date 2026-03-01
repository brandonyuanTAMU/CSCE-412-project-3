/**
 * @file Switch.cpp
 * @brief Implementation of the Switch class.
 */

#include "Switch.h"

/**
 * @brief Constructs a Switch with separate Processing and Streaming load balancers.
 * @param pServers Initial number of servers for Processing.
 * @param sServers Initial number of servers for Streaming.
 * @param cooldown Minimum ticks between scaling adjustments.
 * @param logfile Path to the shared log file.
 * @param blockedIPs Vector of IP prefixes to block.
 */
Switch::Switch(int pServers, int sServers, int cooldown, const std::string& logfile, const std::vector<std::string>& blockedIPs)
    : processingLB(pServers, cooldown, logfile, blockedIPs, "Processing"), streamingLB(sServers, cooldown, logfile, blockedIPs, "Streaming") {}

/**
 * @brief Routes a request to the correct load balancer based on its type.
 *
 * Processing ('P') requests are sent to processingLB, and
 * Streaming ('S') requests are sent to streamingLB.
 *
 * @param req The incoming request to route.
 */
void Switch::addRequest(const Request& req) {
    if (req.getRequestType() == 'P') {
        processingLB.addRequest(req);
    } else {
        streamingLB.addRequest(req);
    }
}

/**
 * @brief Advances both load balancers by one clock cycle.
 */
void Switch::tick() {
    processingLB.tick();
    streamingLB.tick();
}

/**
 * @brief Prints end-of-simulation summaries for both load balancers.
 */
void Switch::end() {
    processingLB.end();
    streamingLB.end();
}