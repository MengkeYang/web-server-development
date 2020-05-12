#ifndef NOT_FOUND_REQUEST_HANDLER_H
#define NOT_FOUND_REQUEST_HANDLER_H

#include "request_handler.h"

class not_found_request_handler : public request_handler
{
public:
    static std::unique_ptr<request_handler> init(const NginxConfig& config);
    response handle_request(const request& req);

};
#endif  // WNZA_NOT_FOUND_REQUEST_HANDLER_H_