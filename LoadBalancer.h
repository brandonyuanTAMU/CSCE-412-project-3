/**
 * @file LoadBalancer.h
 * @brief Defines the LoadBalancer class that manages web servers and a request queue.
 */

#ifndef LOADBALANCER_H
#define LOADBALANCER_H

#include "Request.h"
#include "WebServer.h"

#include <vector>
#include <queue>
#include <string>

/**
 * @class LoadBalancer
 * @brief Manages a pool of web servers and a queue of incoming requests.
 *
 * The LoadBalancer distributes requests to available servers, dynamically
 * scales the server pool based on queue thresholds (50*servers to 80*servers),
 * blocks requests from specified IP ranges, tracks statistics, and logs all
 * events to both the terminal (with color) and a log file.
 */
class LoadBalancer {
    private:
        std::string name;                    ///< Name of this load balancer (e.g., "Processing", "Streaming")
        std::queue<Request> requests;        ///< Queue of pending requests
        std::vector<WebServer> servers;      ///< Pool of active web servers
        int time;                            ///< Current simulation clock cycle
        int lastAdjustedTime;                ///< Last clock cycle when servers were added/removed
        int adjustCooldown;                  ///< Minimum ticks between scaling adjustments
        std::vector<std::string> blockedIPs; ///< List of blocked IP prefixes (firewall)
        std::string logfile;                 ///< Path to the output log file

        // statistics
        int totalRequestsProcessed;  ///< Total number of requests completed
        int totalRequestsReceived;   ///< Total number of requests accepted into the queue
        int totalRequestsBlocked;    ///< Total number of requests rejected by the firewall
        int serversAdded;            ///< Total number of server additions
        int serversRemoved;          ///< Total number of server removals
        int maxServers;              ///< Peak server count during the simulation
        int minServers;              ///< Minimum server count during the simulation
        int maxQueueSize;            ///< Peak queue depth during the simulation

        /**
         * @brief Adds a new server to the pool.
         */
        void addServer();

        /**
         * @brief Removes an idle server from the pool, if one is available.
         */
        void removeServer();

        /**
         * @brief Assigns queued requests to any available servers.
         */
        void assignRequests();

        /**
         * @brief Logs an event without color to both terminal and log file.
         * @param event The event description string.
         */
        void logEvent(const std::string& event) const;

        /**
         * @brief Logs an event with ANSI color to the terminal and plain text to the log file.
         * @param color ANSI color escape code (e.g., "\\033[32m" for green).
         * @param event The event description string.
         */
        void logEvent(const std::string& color, const std::string& event) const;

    public:
        /**
         * @brief Constructs a LoadBalancer with an initial server pool.
         * @param initialservers Number of servers to start with.
         * @param cooldown Minimum clock cycles between scaling adjustments.
         * @param logfile Path to the log file for output.
         * @param blockedips Vector of IP prefixes to block (firewall).
         * @param name Descriptive name for this load balancer.
         */
        LoadBalancer(int initialservers, int cooldown, const std::string& logfile, const std::vector<std::string>& blockedips, const std::string& name);

        /**
         * @brief Advances the simulation by one clock cycle.
         *
         * Processes active servers, checks scaling thresholds, and assigns
         * queued requests to available servers.
         */
        void tick();

        /**
         * @brief Adds a request to the queue if its IP is not blocked.
         * @param req The incoming request.
         */
        void addRequest(const Request& req);

        /**
         * @brief Prints the end-of-simulation summary with statistics.
         */
        void end();
};

#endif