/**
 * @file Request.h
 * @brief Defines the Request class representing a network request.
 */

#ifndef REQUEST_H
#define REQUEST_H

#include <string>

/**
 * @class Request
 * @brief Represents a single network request with source/destination IPs, timing, and type.
 *
 * Each request has an arrival time, a processing duration, source and destination
 * IP addresses, and a job type (Processing or Streaming).
 */
class Request {
    private:
        int arrivalTime;        ///< Clock cycle when the request arrives
        int processingTime;     ///< Number of clock cycles needed to process the request
        std::string ipIn;       ///< Source IP address of the requester
        std::string ipOut;      ///< Destination IP address for the response
        char requestType;       ///< Job type: 'P' for Processing, 'S' for Streaming

    public:
        /**
         * @brief Default constructor. Creates an empty request.
         */
        Request() : arrivalTime(0), processingTime(0), ipIn(""), ipOut(""), requestType('P') {}

        /**
         * @brief Parameterized constructor.
         * @param atime Arrival time in clock cycles.
         * @param ptime Processing time in clock cycles.
         * @param inip Source IP address.
         * @param outip Destination IP address.
         * @param reqtype Job type character ('P' or 'S').
         */
        Request(int atime, int ptime, const std::string& inip, const std::string& outip, char reqtype) :
            arrivalTime(atime), processingTime(ptime), ipIn(inip), ipOut(outip), requestType(reqtype) {}

        /** @brief Gets the arrival time. @return Arrival time in clock cycles. */
        int getArrivalTime() const { return arrivalTime; }

        /** @brief Gets the processing time. @return Processing time in clock cycles. */
        int getProcessingTime() const { return processingTime; }

        /** @brief Gets the source IP address. @return Source IP string. */
        std::string getIpIn() const { return ipIn; }

        /** @brief Gets the destination IP address. @return Destination IP string. */
        std::string getIpOut() const { return ipOut; }

        /** @brief Gets the request type. @return 'P' for Processing or 'S' for Streaming. */
        char getRequestType() const { return requestType; }
};

#endif