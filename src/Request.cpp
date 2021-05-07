
#include "Request.h"

Request::Request(RequestType requestType, CoherenceType coherenceType, long requestAddress) : requestType(requestType),
                                                                                              coherenceType(
                                                                                                      coherenceType),
                                                                                              requestAddress(
                                                                                                      requestAddress) {
    this->requestType = requestType;
    this->coherenceType = coherenceType;
    this->requestAddress = requestAddress;
}

RequestType Request::getRequestType() const {
    return requestType;
}

CoherenceType Request::getCoherenceType() const {
    return coherenceType;
}

long Request::getRequestAddress() const {
    return requestAddress;
}
