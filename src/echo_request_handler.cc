#include "echo_request_handler.h"
#include "request.h"
#include "response.h"
#include <time.h>
#include <iostream>

std::unique_ptr<request_handler> echo_request_handler::init(const NginxConfig& config)
{
    std::unique_ptr<echo_request_handler> er =
                std::make_unique<echo_request_handler>();
    return er;
}

response echo_request_handler::handle_request(const request& req)
{
    response_builder res;
    if (req.method_ != request::INVALID) {
        res.set_code(response::status_code::OK);
        res.add_header("Content-Type", "text/plain");
        std::string methods[] = {"GET", "POST", "PUT", "HEAD"};
        std::string body;
        body += methods[req.method_] + " " + req.uri_ + " " + req.http_version_ + "\r\n";
        for (auto&& entry : req.headers_)
            body += entry.first + ": " + entry.second + "\r\n";
        body += "\r\n";
        body += req.body_;
        res.add_header("Content-Length", std::to_string(body.length()));
        res.add_body(body);
    } else
        res.make_400_error();

    res.make_date_servername_headers();
    return res.get_response();
}
