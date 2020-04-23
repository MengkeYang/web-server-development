#ifndef WNZA_ECHO_REQUEST_HANDLER_H_
#define WNZA_ECHO_REQUEST_HANDLER_H_
#include "request_handler.h"
#include "request.h"
#include "response.h"

class echo_request_handler : public request_handler
{
public:
    void create_response(const request& req, const std::string& raw_data, response& result);
};
#endif  // WNZA_ECHO_REQUEST_HANDLER_H_
