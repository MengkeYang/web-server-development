#ifndef STATUS_REQUEST_HANDLER_H
#define STATUS_REQUEST_HANDLER_H

#include <iostream>
#include <sstream>
#include <iomanip>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include "config_parser.h"
#include "request_handler.h"
#include "request.h"
#include "response.h"

class status_request_handler : public request_handler
{
public:
    static request_handler* init(const std::string& location_path, const NginxConfig& config);
    response handle_request(const request& req);
    std::stringstream get_request_records();

private:
    std::stringstream all_handlers;
    int interval_len;
};

#endif
