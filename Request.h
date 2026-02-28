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
        int getArrivalTime() const { return arrivalTime; }
        int getProcessingTime() const { return processingTime; }
        std::string getIpIn() const { return ipIn; }
        std::string getIpOut() const { return ipOut; }
        char getRequestType() const { return requestType; }
};

#endif