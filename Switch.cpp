#include "Switch.h"

Switch::Switch(int pServers, int sServers, int cooldown, const std::string& logfile, const std::vector<std::string>& blockedIPs)
    : processingLB(pServers, cooldown, logfile, blockedIPs, "Processing"), streamingLB(sServers, cooldown, logfile, blockedIPs, "Streaming") {}

void Switch::addRequest(const Request& req) {
    if (req.getRequestType() == 'P') {
        processingLB.addRequest(req);
    } else {
        streamingLB.addRequest(req);
    }
}

void Switch::tick() {
    processingLB.tick();
    streamingLB.tick();
}

void Switch::end() {
    processingLB.end();
    streamingLB.end();
}