#ifndef PROJECT_CSIM_INTERCONNECTION_H
#define PROJECT_CSIM_INTERCONNECTION_H


#include "Response.h"
#include "Request.h"

class Interconnection {
public:
    virtual int sendRequest(int receiverID, Request request, Response & response);
};


#endif //PROJECT_CSIM_INTERCONNECTION_H
