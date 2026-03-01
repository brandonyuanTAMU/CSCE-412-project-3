/**
 * @file Switch.h
 * @brief Defines the Switch class that routes requests by type to separate load balancers.
 */

#ifndef SWITCH_H
#define SWITCH_H

#include "LoadBalancer.h"
#include "Request.h"
#include <string>
#include <vector>

/**
 * @class Switch
 * @brief A higher-level router that directs requests to type-specific load balancers.
 *
 * The Switch separates incoming requests by job type: Processing ('P') requests
 * go to one LoadBalancer, and Streaming ('S') requests go to another. This allows
 * dedicated server pools for each workload type.
 */
class Switch {
    private:
        LoadBalancer processingLB;  ///< Load balancer for Processing ('P') requests
        LoadBalancer streamingLB;   ///< Load balancer for Streaming ('S') requests

    public:
        /**
         * @brief Constructs a Switch with two load balancers.
         * @param pServers Initial number of servers for the Processing load balancer.
         * @param sServers Initial number of servers for the Streaming load balancer.
         * @param cooldown Minimum ticks between scaling adjustments for each LB.
         * @param logfile Path to the shared log file.
         * @param blockedIPs Vector of IP prefixes to block across both LBs.
         */
        Switch(int pServers, int sServers, int cooldown,
               const std::string& logfile,
               const std::vector<std::string>& blockedIPs);

        /**
         * @brief Routes a request to the appropriate load balancer based on type.
         * @param req The incoming request ('P' goes to Processing, 'S' goes to Streaming).
         */
        void addRequest(const Request& req);

        /**
         * @brief Advances both load balancers by one clock cycle.
         */
        void tick();

        /**
         * @brief Prints end-of-simulation summaries for both load balancers.
         */
        void end();
};

#endif