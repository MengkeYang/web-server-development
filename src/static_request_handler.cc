#include <iostream>
#include "static_request_handler.h"
#include "config_parser.h"
#include <time.h>

// constructor
StaticRequestHandler::StaticRequestHandler(std::string root_uri,
                                           std::string prefix_uri)
{
    int rlen;
    int plen;
    rlen = root_uri.length();
    plen = prefix_uri.length();
    if (root_uri.compare(rlen - 1, 1, "/") != 0)
        root = root_uri + "/";
    else
        root = root_uri;
    if (prefix_uri.compare(plen - 1, 1, "/") != 0)
        prefix = prefix_uri + "/";
    else
        prefix = prefix_uri;
}

std::string StaticRequestHandler::get_file_name(std::string uri)
{
    int i = uri.rfind(prefix);
    if (i != std::string::npos) {
        int len = prefix.length();
        std::cout << "This is the file name: " << uri.substr(i + len)
                  << std::endl;
        return uri.substr(i + len);
    } else {
        // There was no match, this is bad so we should log an error.
        return "404error.html";
    }
}

std::string StaticRequestHandler::extension_to_type(
    const std::string &extension)
{
    if (extension == ".zip") return "application/zip";
    if (extension == ".txt") return "text/plain";
    if (extension == ".jpg") return "image/jpeg";
    if (extension == ".png") return "image/png";
    if (extension == ".html" || extension == ".htm") return "text/html";
    if (extension == ".pdf") return "application/pdf";
    if (extension == ".zip") return "application/zip";
    return "application/octet-stream";  // Default for non-text file
}

void StaticRequestHandler::file_to_body(std::string file_path, response &result)
{
    // check path valid
    std::ifstream file(file_path, std::ios::binary);
    std::string body;
    if (file.is_open()) {
        char c;
        while (file.get(c)) body += c;
        file.close();

        // set MIME
        std::string extension;
        size_t cursor = file_path.rfind(".");
        if (cursor != std::string::npos) extension = file_path.substr(cursor);

        result.add_data(body);
        result.add_header("Content-Type", extension_to_type(extension));
        result.add_header("Content-Length", std::to_string(body.length()));
    } else {
        result.make_404_error();
    }
}

void StaticRequestHandler::create_response(const request &req,
                                           const std::string &raw_data,
                                           response &result)
{
    if (req.parse_result == request_parser::good) {
        result.set_status("200 OK");
        // change path
        std::string filename = get_file_name(req.uri);
        std::string uri = root + filename;

        // Copy the requested file into the response body
        file_to_body(uri, result);
    } else
        result.make_400_error();

    // Setting Date and Server name headers
    char buf[1000];
    memset(buf, 0, sizeof(buf));
    time_t now = time(0);
    struct tm tm = *gmtime(&now);
    strftime(buf, sizeof buf, "%a, %d %b %Y %H:%M:%S %Z", &tm);
    std::string time(buf);
    result.add_header("Date", time);
    result.add_header("Server", "WNZA");
}
