#ifndef REQUEST_H
#define REQUEST_H

#include <string>

class Request {
private:
    int arrivalTime;
    int processingTime;
    std::string ipIn;
    std::string ipOut;
    char requestType;

public:
    Request(int atime, int ptime, const std::string& inip, const std::string& outip, char reqtype) :
        arrivalTime(atime), processingTime(ptime), ipIn(inip), ipOut(outip), requestType(reqtype) {}
    int getArrivalTime() { return arrivalTime; }
    int getProcessingTime() { return processingTime; }
    std::string getIpIn() { return ipIn; }
    std::string getIpOut() { return ipOut; }
    char getRequestType() { return requestType; }
};

#endif