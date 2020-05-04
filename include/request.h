// This is revised from boost HTTP example request.hpp
#ifndef WNZA_REQUEST_H_
#define WNZA_REQUEST_H_

#include <string>
#include <vector>
#include "request_parser.h"

// One header line
struct header {
    std::string name;
    std::string value;
};

/**
 * A request contains all the information from the HTTP request which is
 * necessary in forming a response.
 */
struct request {
    request_parser::result_type parse_result = request_parser::indeterminate;
    std::string method;
    std::string uri;
    int http_version_major;
    int http_version_minor;
    std::vector<header> headers;
    std::string body;
    std::string raw_data;
};
#endif  // WNZA_REQUEST_H_
