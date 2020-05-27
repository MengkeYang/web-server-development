#ifndef REVERSE_PROXY_HANDLER_H
#define REVERSE_PROXY_HANDLER_H

#include <iostream>
#include <istream>
#include <ostream>
#include "string.h"
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include "config_parser.h"
#include "request_handler.h"
#include "request.h"
#include "response.h"
#include "not_found_request_handler.h"
#include "log_helper.h"

class reverse_proxy_request_handler : public request_handler 
{
public:
    reverse_proxy_request_handler(const NginxConfig* config, const std::string& location_path);
    static request_handler* init(const std::string& location_path, const NginxConfig& config);
    response handle_request(const request& req);
    std::string get_handler_name();
private:
    void parse_url(const std::string& url_s);
    response send_request(const request& req);
    response handle_redirect(const request& req, std::string location);
    std::string request_to_string(const request& req);
    request get_proxy_request(const request& req, std::string uri);
    response throw_400_error();
    std::string dest_, port_, base_uri_, protocol_, host_, path_, query_, location_path_;
};

#endif