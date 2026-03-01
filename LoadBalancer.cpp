/**
 * @file LoadBalancer.cpp
 * @brief Implementation of the LoadBalancer class.
 *
 * Handles request queuing, server management, dynamic scaling,
 * IP-based firewall blocking, colored terminal output, and
 * end-of-simulation statistics reporting.
 */

#include "LoadBalancer.h"
#include "Request.h"
#include "WebServer.h"

#include <iostream>
#include <fstream>
#include <algorithm>

/// @name ANSI Color Codes
/// @brief Used for colored terminal output to distinguish event types.
///@{
#define CLR_RESET   "\033[0m"
#define CLR_GREEN   "\033[32m"
#define CLR_RED     "\033[31m"
#define CLR_YELLOW  "\033[33m"
#define CLR_CYAN    "\033[36m"
#define CLR_MAGENTA "\033[35m"
#define CLR_BOLD    "\033[1m"
///@}

/**
 * @brief Constructs a LoadBalancer with the given configuration.
 *
 * Initializes the server pool, sets up the firewall, and zeroes
 * all statistics counters.
 *
 * @param initservers Initial number of web servers to create.
 * @param cooldown Minimum ticks between scaling adjustments.
 * @param logfile Path to the output log file.
 * @param blockedips Vector of IP prefixes to block.
 * @param name Display name for this load balancer instance.
 */
LoadBalancer::LoadBalancer(int initservers, int cooldown, const std::string& logfile, const std::vector<std::string>& blockedips, const std::string& name)
    : time(0), lastAdjustedTime(0), adjustCooldown(cooldown), logfile(logfile), blockedIPs(blockedips), name(name),
      totalRequestsProcessed(0), totalRequestsReceived(0), totalRequestsBlocked(0),
      serversAdded(0), serversRemoved(0), maxServers(initservers), minServers(initservers), maxQueueSize(0) {
    for (int i=0; i<initservers; ++i) {
        servers.push_back(WebServer());
    }
    logEvent(CLR_CYAN, "LoadBalancer initialized with num servers: " + std::to_string(initservers));
}

/**
 * @brief Logs an event without color formatting.
 * @param event The event description string.
 */
void LoadBalancer::logEvent(const std::string& event) const {
    logEvent("", event);
}

/**
 * @brief Logs an event with ANSI color to the terminal and plain text to the log file.
 *
 * Terminal output includes color codes for visual distinction of event types.
 * Log file output omits ANSI codes for clean text storage.
 *
 * @param color ANSI escape code for the terminal color.
 * @param event The event description string.
 */
void LoadBalancer::logEvent(const std::string& color, const std::string& event) const {
    // colored output to terminal
    std::cout << color << "[" << name << "][t=" << time << "] " << event << CLR_RESET << "\n";

    // plain text to log file (no ANSI codes)
    std::ofstream file(logfile, std::ios::app);
    if (file.is_open()) {
        file << "[" << name << "][t=" << time << "] " << event << "\n";
    }
}

/**
 * @brief Adds a new web server to the pool.
 *
 * Called when the queue size exceeds 80 * number of servers.
 * Updates the serversAdded counter and maxServers tracker.
 */
void LoadBalancer::addServer() {
    servers.push_back(WebServer());
    serversAdded++;
    if ((int)servers.size() > maxServers) maxServers = servers.size();
    logEvent(CLR_GREEN, "Server added, total servers: " + std::to_string(servers.size()));
}

/**
 * @brief Removes an idle server from the pool.
 *
 * Called when the queue size drops below 50 * number of servers.
 * Only removes a server that is currently available (not processing a request).
 * Updates the serversRemoved counter and minServers tracker.
 */
void LoadBalancer::removeServer() {
    for (size_t i = 0; i < servers.size(); i++) {
        if (servers[i].getAvailability()) {
            servers.erase(servers.begin() + i);
            serversRemoved++;
            if ((int)servers.size() < minServers) minServers = servers.size();
            logEvent(CLR_YELLOW, "Server removed, total servers: " + std::to_string(servers.size()));
            return;
        }
    }
    logEvent(CLR_YELLOW, "Couldn't remove a server (all busy)");
}

/**
 * @brief Distributes queued requests to available servers.
 *
 * Iterates through all servers and assigns the next request from
 * the queue to each available server.
 */
void LoadBalancer::assignRequests() {
    for (WebServer& server : servers) {
        if (server.getAvailability() && !requests.empty()) {
            server.assignRequest(requests.front());
            requests.pop();
        }
    }
}

/**
 * @brief Adds a request to the queue after checking the IP firewall.
 *
 * If the request's source IP matches any blocked prefix, the request
 * is rejected and logged. Otherwise, it is added to the queue.
 *
 * @param req The incoming request to process.
 */
void LoadBalancer::addRequest(const Request& req) {
    for (const std::string& blocked : blockedIPs) {
        if (req.getIpIn().rfind(blocked, 0) == 0) { // check for prefix
            totalRequestsBlocked++;
            logEvent(CLR_RED, "Blocked request from IP: " + req.getIpIn());
            return;
        }
    }
    totalRequestsReceived++;
    requests.push(req);
    if ((int)requests.size() > maxQueueSize) maxQueueSize = requests.size();
}

/**
 * @brief Advances the load balancer by one clock cycle.
 *
 * Each tick performs three actions in order:
 * 1. Ticks all busy servers and logs completed requests.
 * 2. Checks if the scaling cooldown has elapsed, then adds or
 *    removes servers based on queue-to-server ratio thresholds.
 * 3. Assigns queued requests to any newly available servers.
 */
void LoadBalancer::tick() {
    ++time;
    for (WebServer& server : servers) {
        if (!server.getAvailability()) {
            bool finished = server.tick();
            if (finished) {
                totalRequestsProcessed++;
                logEvent("Request completed at time " + std::to_string(time));
            }
        }
    }
    if (time - lastAdjustedTime >= adjustCooldown) {
        if (requests.size() > 80 * servers.size()) {
            addServer();
            lastAdjustedTime = time;
        } else if (requests.size() < 50 * servers.size() && servers.size() > 1) {
            removeServer();
            lastAdjustedTime = time;
        }
    }
    assignRequests();
}

/**
 * @brief Prints a comprehensive end-of-simulation summary.
 *
 * Outputs key statistics including total requests processed, blocked,
 * remaining in queue, server scaling history, and peak values.
 * Summary is printed to both terminal (with color) and log file.
 */
void LoadBalancer::end() {
    std::string sep = "========================================";
    logEvent(CLR_BOLD, sep);
    logEvent(CLR_BOLD, "  SUMMARY: " + name + " Load Balancer");
    logEvent(CLR_BOLD, sep);
    logEvent(CLR_CYAN,    "  Simulation duration:      " + std::to_string(time) + " ticks");
    logEvent(CLR_CYAN,    "  Total requests received:  " + std::to_string(totalRequestsReceived));
    logEvent(CLR_GREEN,   "  Total requests processed: " + std::to_string(totalRequestsProcessed));
    logEvent(CLR_RED,     "  Total requests blocked:   " + std::to_string(totalRequestsBlocked));
    logEvent(CLR_YELLOW,  "  Remaining in queue:       " + std::to_string(requests.size()));
    logEvent(CLR_MAGENTA, "  Servers added:            " + std::to_string(serversAdded));
    logEvent(CLR_MAGENTA, "  Servers removed:          " + std::to_string(serversRemoved));
    logEvent(CLR_CYAN,    "  Final server count:       " + std::to_string(servers.size()));
    logEvent(CLR_CYAN,    "  Max server count:         " + std::to_string(maxServers));
    logEvent(CLR_CYAN,    "  Min server count:         " + std::to_string(minServers));
    logEvent(CLR_CYAN,    "  Max queue size:           " + std::to_string(maxQueueSize));
    logEvent(CLR_BOLD, sep);
}