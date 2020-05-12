#include <iostream>
#include "static_request_handler.h"
#include "config_parser.h"
#include <time.h>

// constructor
static_request_handler::static_request_handler(const NginxConfig* config, std::string prefix_uri)
{
    int rlen;
    std::string root_uri;
    int plen;
    root_uri = config->get_value_from_statement("root");
    if (root_uri.length() != 0)
        root_uri = root_uri.substr(1, root_uri.length() - 2);
    rlen = root_uri.length();
    plen = prefix_uri.length();
    if (root_uri.compare(rlen - 1, 1, "/") != 0)
        root_ = root_uri + "/";
    else
        root_ = root_uri;
    if (prefix_uri.compare(plen - 1, 1, "/") != 0)
        prefix_ = prefix_uri + "/";
    else
        prefix_ = prefix_uri;
}

request_handler* static_request_handler::init(const std::string& location_path, const NginxConfig& config)
{
    //return new static_request_handler(&config, location_path);
    
   if(!config.get_value_from_statement("root").empty()){
        return new static_request_handler(&config, location_path);
   }else{
        // default root_path and uri
        NginxConfig* default_config = new NginxConfig;
        std::shared_ptr<NginxConfigStatement> statement(new NginxConfigStatement); 
        statement->tokens_.push_back("root");
        statement->tokens_.push_back("\".\";");
        default_config->statements_.push_back(statement);
        return new static_request_handler(default_config, "/static");
   }
}

std::string static_request_handler::get_file_name(std::string uri)
{
    int i = uri.rfind(prefix_);
    if (i != std::string::npos) {
        int len = prefix_.length();
        std::cout << "This is the file name: " << uri.substr(i + len)
                  << std::endl;
        return uri.substr(i + len);
    } else {
        // There was no match, this is bad so we should log an error.
        return "404error.html";
    }
}

std::string static_request_handler::extension_to_type(
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

void static_request_handler::file_to_body(std::string file_path, response_builder &result)
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

        result.add_body(body);
        result.add_header("Content-Type", extension_to_type(extension));
        result.add_header("Content-Length", std::to_string(body.length()));
    } else {
        result.make_404_error();
    }
}

response static_request_handler::handle_request(const request &req)
{
    response_builder res;
    if (req.method_ != request::INVALID) {
        res.set_code(response::status_code::OK);
        // change path
        std::string filename = get_file_name(req.uri_);
        std::string uri = root_ + filename;

        // Copy the requested file into the response body
        file_to_body(uri, res);
    } else
        res.make_400_error();

    res.make_date_servername_headers();
    return res.get_response();
}
