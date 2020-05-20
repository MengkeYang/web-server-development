#include <iostream>
#include "static_request_handler.h"
#include "config_parser.h"
#include "not_found_request_handler.h"
#include <time.h>

// constructor
static_request_handler::static_request_handler(const NginxConfig* config, std::string prefix_uri)
{
    int rlen;
    int plen;
    std::string root_uri;
    root_uri = config->get_value_from_statement("root");

    // If the scoped block has a root, then use that as the root
    // WARNING: If no root is present, we use "." as the default!
    //          This should not happen.
    std::cout << "Config URI: " << root_uri << std::endl;
    if (root_uri.length() != 0)
        root_uri = root_uri.substr(1, root_uri.length() - 2);
    else
        root_uri = ".";

    rlen = root_uri.length();
    plen = prefix_uri.length();

    // Ensure that the root we store always ends in "/"
    if (root_uri.compare(rlen - 1, 1, "/") != 0)
        root_ = root_uri + "/";
    else
        root_ = root_uri;

    // Ensure the handlers URI prefix always ends in "/"
    if (prefix_uri.compare(plen - 1, 1, "/") != 0)
        prefix_ = prefix_uri + "/";
    else
        prefix_ = prefix_uri;
}

request_handler* static_request_handler::init(const std::string& location_path,
                                              const NginxConfig& config)
{
    if (!config.get_value_from_statement("root").empty()) {
        return new static_request_handler(&config, location_path);
    } else {
        return not_found_request_handler::init(location_path, config);
    }
}

// Returns the a filename if the uri mapped to a valid file.
// Otherwise, returns empty string.
std::string static_request_handler::get_file_name(std::string uri)
{
    int i = uri.rfind(prefix_);
    if (i != std::string::npos) {
        int len = prefix_.length();
        return uri.substr(i + len);
    } else {
        // There was no match, this is bad so we should log an error.
        return "";
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

bool static_request_handler::file_to_body(std::string file_path, response_builder &result)
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
        return true;
    } else {
        return false;
    }
}

response static_request_handler::handle_request(const request &req)
{
    response_builder res;
    res.set_code(response::status_code::OK);

    // change path
    std::string filename = get_file_name(req.uri_);
    std::string uri = root_ + filename;

    // Copy the requested file into the response body
    if (filename.empty() || !file_to_body(uri, res)) {
        return not_found_request_handler::handle_request(req);
    }

    res.make_date_servername_headers();
    return res.get_response();
}
