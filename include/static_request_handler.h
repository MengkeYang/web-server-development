#ifndef STATIC_REQUEST_HANDLER_H
#define STATIC_REQUEST_HANDLER_H

#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include "config_parser.h"
#include "request_handler.h"
#include "request.h"
#include "response.h"

class StaticRequestHandler : public request_handler
{
public:
    StaticRequestHandler(std::string root, std::string prefix);
    void create_response(const request& req, const std::string& raw_data,
                         response& result);

private:
    // for map path
    std::string root;
    std::string prefix;
    std::string get_file_name(std::string uri);
    std::string extension_to_type(const std::string& extension);
};

#endif
