#include "response.h"
#include <iostream>
#include <string>

std::vector<boost::asio::const_buffer> response::getResponse()
{
    std::vector<boost::asio::const_buffer> resp;
    resp.push_back(boost::asio::buffer(header));
    resp.push_back(boost::asio::buffer(body));
    return resp;
}

void response::addResponse(request_parser::result_type r,
                           std::string& b)
{
    std::string reply;
    switch (r) {
        case request_parser::good: {
            std::size_t len = b.length();
            reply =
                "HTTP/1.1 200 OK\r\nContent-Length: " + std::to_string(len) +
                "\r\nContent-Type: text/plain\r\n\r\n";
            header = reply;
            body = b;
            break;
        }
        case request_parser::bad: {
            reply = "HTTP/1.1 400 Bad Request\r\n\r\n";
            header = reply;
            break;
        }
        case request_parser::indeterminate: {
            std::cerr << "Error: Not able to parse to HTTP request\n";
            reply = "HTTP/1.1 400 Bad Request\r\n\r\n";
            header = reply;
            break;
        }
    }
}
