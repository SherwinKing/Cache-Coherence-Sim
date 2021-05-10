#include "Response.h"


Response::Response(ResponseType responseType) : responseType(responseType) {
    this->responseType = responseType;
    tokenNum = 0;
}

void Response::setCacheOwnerIDs(const std::list<int> &cacheOwnerIDs) {
    assert(responseType == DIRECTORY_RESPONSE);
    Response::cacheOwnerIDs = cacheOwnerIDs;
}

void Response::setTokenNum(int tokenNum) {
    assert(responseType == TOKEN_RESPONSE);
    Response::tokenNum = tokenNum;
}

ResponseType Response::getResponseType() const {
    return responseType;
}

const std::list<int> &Response::getCacheOwnerIDs() const {
    return cacheOwnerIDs;
}

int Response::getTokenNum() const {
    return tokenNum;
}
