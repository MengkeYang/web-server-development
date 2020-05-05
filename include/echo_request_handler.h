#ifndef WNZA_ECHO_REQUEST_HANDLER_H_
#define WNZA_ECHO_REQUEST_HANDLER_H_
#include "request_handler.h"

class request;
class response;

class echo_request_handler : public request_handler
{
public:
    void create_response(const request& req, response& result);
};
#endif  // WNZA_ECHO_REQUEST_HANDLER_H_