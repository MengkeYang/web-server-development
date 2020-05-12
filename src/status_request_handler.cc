#include <iostream>
#include "status_request_handler.h"
#include "config_parser.h"


status_request_handler::status_request_handler(std::vector<location_parse_result> loc_res)
{
    handlers_info_ = loc_res;
}

std::unique_ptr<request_handler> status_request_handler::init(const NginxConfig& config)
{
    std::unique_ptr<status_request_handler> sr =
                std::make_unique<status_request_handler>(config.get_location_result());
    return sr;
}

response status_request_handler::create_response(const request& req)
{
    response res;
    if (req.method_ != request::INVALID) {
        res.set_code(response::status_code::OK);
        res.add_header("Content-Type", "text/plain");
        std::string body;
        body += "handlers name \t URL prefixes\r\n";
        for (location_parse_result loc_res : handlers_info_)
            body += loc_res.handler_name + " \t " + loc_res.uri + "\r\n";
        res.add_header("Content-Length", std::to_string(body.length()));
        res.add_body(body);
    } else
        res.make_400_error();

    res.make_date_servername_headers();
    return res;
}