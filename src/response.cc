#include "response.h"
#include "request.h"
#include <iostream>
#include <string>
#include <memory>
#include <boost/filesystem/fstream.hpp>

void response_builder::set_response(response& res) {
    response_ = res;
}

response response_builder::get_response() { return response_; }

/**
 * Construct an HTTP response message according to HTTP 1.1 based on the values
 * that were setup from the add_header and add_data methods. The return value
 * can be directly passed to async_write.
 */
std::vector<boost::asio::const_buffer> response_builder::build()
{
    std::vector<boost::asio::const_buffer> resp;
    std::shared_ptr<std::string> entire_header = std::make_shared<std::string>();
    *entire_header += "HTTP/1.1 ";
    switch (response_.code_) {
        case response::status_code::OK: *entire_header += "200 OK"; break;
        case response::status_code::BAD_REQ: *entire_header += "400 Bad Request"; break;
        case response::status_code::NOT_FOUND: *entire_header += "404 Not Found"; break;
    }
    *entire_header += "\r\n";

    for (auto&& h : response_.headers_) *entire_header += h.first + ": " + h.second + "\r\n";
    *entire_header += "\r\n";

    head_ = entire_header;
    body_ = std::make_shared<std::string>(response_.body_);

    resp.push_back(boost::asio::buffer(*head_));
    resp.push_back(boost::asio::buffer(*body_));
    bufs_ = resp;

    return bufs_;
}

/**
 * Use add_header to add information to the final response message which will
 * be sent along with the data. The "Content-Length" header and "Content-Type"
 * header must be added if data is being sent. This will be left to the sender
 * to coordinate.
 */
void response_builder::add_header(std::string key, std::string val)
{
    response_.headers_[key] = val;
}

void response_builder::add_body(std::string data) { response_.body_ = data; }

void response_builder::set_code(response::status_code code)
{
    response_.code_ = code;
}

void response_builder::make_400_error()
{
    set_code(response::status_code::BAD_REQ);
    add_header("Content-Length", "0");
    add_body("");
}

void response_builder::make_404_error()
{
    set_code(response::status_code::NOT_FOUND);
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

void response_builder::make_date_servername_headers()
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
