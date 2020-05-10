#include "echo_request_handler.h"
#include "request.h"
#include "response.h"
#include <time.h>

std::unique_ptr<request_handler> echo_request_handler::init(const NginxConfig& config)
{
    std::unique_ptr<echo_request_handler> er =
                std::make_unique<echo_request_handler>();
    return er;
}

void echo_request_handler::create_response(const request& req, response& result)
{
    if (req.parse_result == request_parser::good) {
        result.set_status("200 OK");
        result.add_header("Content-Length", std::to_string(req.raw_data.length()));
        result.add_header("Content-Type", "text/plain");
        result.add_data(req.raw_data);
    } else
        result.make_400_error();

    result.make_date_servername_headers();
}
