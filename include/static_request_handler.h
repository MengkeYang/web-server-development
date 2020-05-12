#ifndef STATIC_REQUEST_HANDLER_H
#define STATIC_REQUEST_HANDLER_H

#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include "config_parser.h"
#include "request_handler.h"
#include "request.h"
#include "response.h"

class static_request_handler : public request_handler
{
public:
    static_request_handler(std::string root_uri, std::string prefix_uri);
    static request_handler* init(const NginxConfig& config);
    response handle_request(const request& req);

private:
    // for map path
    std::string root_;
    std::string prefix_;
    std::string get_file_name(std::string uri);
    std::string extension_to_type(const std::string& extension);
    void file_to_body(std::string uri, response_builder &result);
};

#endif
