#include "SnoopingCacheController.h"

void SnoopingCacheController::runCacheOp(long tag, long setInd, char operation) {
    return;
}

/**
 * Cache hears requests on bus.
 * @param request
 * @return
 */
Response SnoopingCacheController::deliverRequest(Request request) {
    cache.stateTransition(request.getRequestAddress(), request.getRequestType());
    Response response = Response(ACK);
    return response;
}
