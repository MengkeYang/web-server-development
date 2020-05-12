#ifndef STATUS_REQUEST_HANDLER_H
#define STATUS_REQUEST_HANDLER_H

#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include "config_parser.h"
#include "request_handler.h"
#include "request.h"
#include "response.h"

class status_request_handler : public request_handler
{
public:
    status_request_handler(std::vector<location_parse_result> loc_res);
    static std::unique_ptr<request_handler> init(const NginxConfig& config);
    response create_response(const request& req);


private:
    std::vector<location_parse_result> handlers_info_;
};

#endif