// This is revised from boost HTTP example request.hpp
#ifndef WNZA_REQUEST_H_
#define WNZA_REQUEST_H_

#include <string>
#include <vector>
#include <map>
#include "request_parser.h"

/**
 * A request contains all the information from the HTTP request which is
 * necessary in forming a response.
 */
struct request {
    enum method { GET = 0, POST = 1, PUT = 2, HEAD = 3, INVALID = 4 } method_;

    std::string uri_;
    std::string version_;
    std::map<std::string, std::string> headers_;
    std::string body_;
};
#endif  // WNZA_REQUEST_H_
