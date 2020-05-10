#include "response.h"
#include "request.h"
#include <iostream>
#include <string>
#include <memory>
#include <boost/filesystem/fstream.hpp>

/**
 * Construct an HTTP response message according to HTTP 1.1 based on the values
 * that were setup from the add_header and add_data methods. The return value
 * can be directly passed to async_write.
 */
buffer_response response::build_response()
{
    std::vector<boost::asio::const_buffer> resp;
    std::shared_ptr<std::string> entire_header = std::make_shared<std::string>();
    *entire_header += "HTTP/1.1 ";
    switch (code_) {
        case OK: *entire_header += "200 OK"; break;
        case BAD_REQ: *entire_header += "400 Bad Request"; break;
        case NOT_FOUND: *entire_header += "404 Not Found"; break;
    }
    *entire_header += "\r\n";

    for (auto&& h : headers_) *entire_header += h.first + ": " + h.second + "\r\n";
    *entire_header += "\r\n";

    buffer_response result;
    result.head = entire_header;
    result.body = std::make_shared<std::string>(body_);

    resp.push_back(boost::asio::buffer(*result.head));
    resp.push_back(boost::asio::buffer(*result.body));
    result.bufs = resp;

    return result;
}

/**
 * Use add_header to add information to the final response message which will
 * be sent along with the data. The "Content-Length" header and "Content-Type"
 * header must be added if data is being sent. This will be left to the sender
 * to coordinate.
 */
void response::add_header(std::string key, std::string val)
{
    headers_[key] = val;
}

void response::add_body(std::string data) { body_ = data; }

void response::set_code(status_code code)
{
    code_ = code;
}

void response::make_400_error()
{
    set_code(status_code::BAD_REQ);
    add_header("Content-Length", "0");
    add_body("");
}

void response::make_404_error()
{
    set_code(status_code::NOT_FOUND);
    std::ifstream file("404page.html", std::ios::binary);
    std::string body;
    if (file.is_open()) {
        char c;
        while (file.get(c)) body += c;
        file.close();
    }
    add_body(body);
    add_header("Content-Type", ".html");
    add_header("Content-Length", std::to_string(body.length()));

}

void response::make_date_servername_headers()
{
   // Setting Date and Server name headers
    char buf[1000];
    memset(buf, 0, sizeof(buf));
    time_t now = time(0);
    struct tm tm = *gmtime(&now);
    strftime(buf, sizeof buf, "%a, %d %b %Y %H:%M:%S %Z", &tm);
    std::string time(buf);
    add_header("Date", time);
    add_header("Server", "WNZA");
}
