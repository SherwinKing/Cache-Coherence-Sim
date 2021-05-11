
#ifndef PROJECT_CSIM_REQUEST_H
#define PROJECT_CSIM_REQUEST_H


enum RequestType { BusRd, BusRdX, TOKEN_REQUEST, DIRECTORY_REQUEST_R, 
                   DIRECTORY_REQUEST_W };
enum CoherenceType { SNOOPING, TOKEN, DIRECTORY };

class Request {
private:
    RequestType requestType;
    CoherenceType coherenceType;
    long requestAddress;

public:
    Request(RequestType requestType, CoherenceType coherenceType, long requestAddress);

    RequestType getRequestType() const;

    CoherenceType getCoherenceType() const;

    long getRequestAddress() const;
};


#endif //PROJECT_CSIM_REQUEST_H
