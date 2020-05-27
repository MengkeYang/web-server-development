#include "not_found_request_handler.h"
#include "request.h"
#include "response.h"
#include <time.h>
#include <iostream>

request_handler* not_found_request_handler::init(const std::string& location_path, const NginxConfig& config)
{
    return new not_found_request_handler();
}

response not_found_request_handler::handle_request(const request& req)
{
    response_builder res;
    res.make_404_error();
    return res.get_response();
}

std::string not_found_request_handler::get_handler_name(){
    return "NotFoundHandler";
}