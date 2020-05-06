#include "response.h"
#include "request.h"
#include <iostream>
#include <string>
#include <boost/filesystem/fstream.hpp>

/**
 * Construct an HTTP response message according to HTTP 1.1 based on the values
 * that were setup from the add_header and add_data methods. The return value
 * can be directly passed to async_write.
 */
std::vector<boost::asio::const_buffer> response::build_response()
{
    std::vector<boost::asio::const_buffer> resp;
    entire_header = "HTTP/1.1 ";
    entire_header += return_code + "\r\n";
    for (auto& h : headers) entire_header += h.name + ": " + h.value + "\r\n";
    entire_header += "\r\n";
    resp.push_back(boost::asio::buffer(entire_header));

    resp.push_back(boost::asio::buffer(body));
    return resp;
}

/**
 * Use add_header to add information to the final response message which will
 * be sent along with the data. The "Content-Length" header and "Content-Type"
 * header must be added if data is being sent. This will be left to the sender
 * to coordinate.
 */
void response::add_header(std::string key, std::string val)
{
    std::string name(key);
    std::string value(val);
    header h = {name, value};
    headers.push_back(h);
}

void response::add_data(std::string data) { body = data; }

void response::set_status(std::string status_code)
{
    return_code = status_code;
}

void response::make_400_error()
{
    set_status("400 Bad Request");
    add_header("Content-Length", "0");
    add_data("");
}

void response::make_404_error()
{
    set_status("404 Not Found");
    std::ifstream file("404page.html", std::ios::binary);
    std::string body;
    if (file.is_open()) {
        char c;
        while (file.get(c)) body += c;
        file.close();
    }
    add_data(body);
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
