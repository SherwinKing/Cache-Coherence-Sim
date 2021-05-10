
#ifndef PROJECT_CSIM_RESPONSE_H
#define PROJECT_CSIM_RESPONSE_H

#include<list>
#include <cassert>

enum ResponseType { ACK, DIRECTORY_RESPONSE, TOKEN_RESPONSE};

class Response {
private:
    ResponseType responseType;
    std::list<int> cacheOwnerIDs;
    int tokenNum;

public:
    Response(ResponseType responseType);

    void setCacheOwnerIDs(const std::list<int> &cacheOwnerIDs);

    void setTokenNum(int tokenNum);

    ResponseType getResponseType() const;

    const std::list<int> &getCacheOwnerIDs() const;

    int getTokenNum() const;
};


#endif //PROJECT_CSIM_RESPONSE_H
