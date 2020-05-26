#ifndef WNZA_HEALTH_REQUEST_HANDLER_H_
#define WNZA_HEALTH_REQUEST_HANDLER_H_
#include "request_handler.h"

class request;
class response;

class health_request_handler : public request_handler
{
public:
    static request_handler* init(const std::string& location_path, const NginxConfig& config);
    response handle_request(const request& req);
};
#endif  // WNZA_HEALTH_REQUEST_HANDLER_H_
