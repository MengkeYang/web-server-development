#include "health_request_handler.h"
#include "request.h"
#include "response.h"
#include <time.h>
#include <iostream>

request_handler* health_request_handler::init(const std::string& location_path, const NginxConfig& config)
{
    return new health_request_handler();
}

response health_request_handler::handle_request(const request& req)
{
    response_builder res;
    if (req.method_ != request::INVALID) {
        res.set_code(response::status_code::OK);
        res.add_header("Content-Length", "0");
    } else
        res.make_400_error();

    res.make_date_servername_headers();
    return res.get_response();
}


std::string health_request_handler::get_handler_name(){
    return "HealthHandler";
}