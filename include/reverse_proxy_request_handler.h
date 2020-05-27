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

struct url;

class reverse_proxy_request_handler : public request_handler 
{
public:
    reverse_proxy_request_handler(const NginxConfig* config, const std::string& prefix_uri);
    static request_handler* init(const std::string& prefix_uri, const NginxConfig& config);
    response handle_request(const request& req);
    std::string get_handler_name();
private:
    url parse_url(const std::string& url_s);
    response send_request(const request& req, std::string host);
    response handle_redirect(const request& req, std::string location, url prev_loc);
    std::string request_to_string(const request& req);
    request get_proxy_request(const request& req, url uri);
    response throw_400_error();
    std::string dest_, port_, base_uri_, protocol_, host_, path_, query_, prefix_;
};

struct url {
    std::string host_;
    std::string path_;
    std::string query_;
};

#endif
