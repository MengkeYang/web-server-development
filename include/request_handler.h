#ifndef WNZA_REQUEST_HANDLER_H_
#define WNZA_REQUEST_HANDLER_H_
#include <string>

class request;
class response;

class request_handler
{
public:
    // virtual static request_handler init(const NginxConfig& config) = 0；
    virtual void create_response(const request& req, response& result) = 0;
    virtual ~request_handler() {};
};
#endif  // WNZA_REQUEST_HANDLER_H_
