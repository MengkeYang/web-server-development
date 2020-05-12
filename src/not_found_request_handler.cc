#include "not_found_request_handler.h"
#include "request.h"
#include "response.h"
#include <time.h>
#include <iostream>

std::unique_ptr<request_handler> not_found_request_handler::init(const NginxConfig& config)
{
    std::unique_ptr<not_found_request_handler> er =
                std::make_unique<not_found_request_handler>();
    return er;
}

response not_found_request_handler::handle_request(const request& req)
{
    response_builder res;
    res.make_404_error();
    return res.get_response();
}