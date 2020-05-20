#include <iostream>
#include "string.h"
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include "config_parser.h"
#include "request_handler.h"
#include "request.h"
#include "response.h"
#include "not_found_request_handler.h"
#include "reverse_proxy_request_handler.h"
#include <string>
#include <algorithm>
#include <cctype>
#include <functional>


using boost::asio::buffers_begin;
using boost::asio::ip::tcp;


reverse_proxy_request_handler::reverse_proxy_request_handler(const NginxConfig* config)
{
    dest_ = config->get_value_from_statement("dest");
    dest_ = dest_.substr(1, dest_.length() - 2);

    // get the port number from the config block, or assign a default port value
    if (!config->get_value_from_statement("port").empty()) {
        port_ = config->get_value_from_statement("port");
    } else {
        port_ = "80";
    }
    // parse out the parts of our specified destination
    parse_url(dest_);
}

request_handler* reverse_proxy_request_handler::init(const std::string& location_path,
                                              const NginxConfig& config)
{
    if (!config.get_value_from_statement("dest").empty()) {
        return new reverse_proxy_request_handler(&config);
    } else {
        return not_found_request_handler::init(location_path, config);
    }
}

response reverse_proxy_request_handler::handle_request(const request& req)
{
    // verify that the host and path we're about to contact exist
    if (host_ == "" || path_ == "") {
        // TODO: Log that the dest is malformed
        return throw_400_error();
    }
    // find the second '/' character to retrieve the path
    size_t index = req.uri_.find_first_of("/", 1);
    if (index == std::string::npos) {
        // no path specified
        request new_req = get_proxy_request(req, "");
        return send_request(new_req);
    } else {
        std::string new_uri = req.uri_.substr(index, req.uri_.length());
        request new_req = get_proxy_request(req, new_uri);
        return send_request(new_req);
    }   
}

// for reference:
// https://www.boost.org/doc/libs/1_50_0/doc/html/boost_asio/example/http/client/sync_client.cpp
response reverse_proxy_request_handler::send_request(const request& req) {
    response_builder res;
    // obtain the request as a string
    std::string curr_request = request_to_string(req);
    // an instance of the io service to send the request
    boost::asio::io_service io_service;
    boost::system::error_code ec;
    
    // get a list of endpoints corresponding to the server name.
    tcp::resolver resolver(io_service);
    tcp::resolver::query query(host_, port_);
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

    // try each endpoint until we successfully establish a connection.
    tcp::socket socket(io_service);
    boost::asio::connect(socket, endpoint_iterator, ec);
    if (ec) { 
        // TODO: Log an error here
        return throw_400_error();
    }
    
    // send out the request and read the response
    boost::asio::streambuf request_buffer;
    std::ostream request_stream(&request_buffer);
    request_stream << curr_request;

    boost::asio::write(socket, request_buffer, ec);
    boost::asio::streambuf response_buffer;
    boost::asio::read_until(socket, response_buffer, "\r\n", ec);

    // check that the response is OK
    std::istream response_stream(&response_buffer);
    std::string http_version;
    response_stream >> http_version;
    unsigned int status_code;
    response_stream >> status_code;
    std::string status_message;
    std::getline(response_stream, status_message); // contains a space in front of it!
    if (!response_stream || http_version.substr(0, 5) != "HTTP/"){
        // TODO: Log an error here
        return throw_400_error();
    }
    
    std::string header;
    // handle redirection
    if (status_code == 302 || status_code == 301) {
        while (std::getline(response_stream, header) && header != "\r") {
            if (header.find("Location") != std::string::npos) {
                header.pop_back();
                std::string location = header.substr(header.find(":")+2);
                return handle_redirect(req, location);
            }
        }
        // TODO: Log an error here, no redirect location was provided
        return throw_400_error();
    } else if (status_code == 200) {
        res.set_code(response::status_code::OK);
    } else if (status_code == 404) {
        res.make_404_error();
        res.make_date_servername_headers();
        return res.get_response();
    } else {
        return throw_400_error();
    }
    
    // read the response headers, which are terminated by a blank line
    boost::asio::read_until(socket, response_buffer, "\r\n\r\n", ec);

    // process the response headers.
    while (std::getline(response_stream, header) && header != "\r"){
        std::string token = header.substr(0, header.find(":"));
        res.add_header(token, header.substr(header.find(":")+2));
    }
    
    // write whatever content we already have to the body.
    std::string body;
    std::string content;
    if (response_buffer.size() > 0){
        std::getline(response_stream, content);
    }
    body += content;

    // read until EOF, writing data to a content string so we can finish off our body.
    boost::system::error_code error;
    content = "";
    while (boost::asio::read(socket, response_buffer, boost::asio::transfer_at_least(1), error)){
        auto bufs = response_buffer.data();
        std::string content_string(buffers_begin(bufs), buffers_begin(bufs) + response_buffer.size());
        content = content_string;
    }
    body += content;

    if (error != boost::asio::error::eof)
      throw boost::system::system_error(error);
      
    // add on final content-length header, body, and final fields to prepare the response.
    res.add_header("Content-Length", std::to_string(body.length()));
    res.add_body(body);
    res.make_date_servername_headers();
    return res.get_response();
}

// create the new redirect request object
response reverse_proxy_request_handler::handle_redirect(const request& req, std::string location) {
    // clear the params from the previous location
    protocol_ = "";
    query_ = "";
    host_ = "";
    path_ = "";
    // format the location so it's ready for the parser, obtain the new params of the location
    if(location.find("http://") != std::string::npos || location.find("https://") != std::string::npos) {
        parse_url(location);  
    } else {
        location = "http://" + location;
        parse_url(location);
    }

    // obtain the new request and send it
    request new_req = get_proxy_request(req, "");
    return send_request(new_req);
}

// referenced from https://stackoverflow.com/questions/2616011/easy-way-to-parse-a-url-in-c-cross-platform
void reverse_proxy_request_handler::parse_url(const std::string& url_s)
{
    const std::string prot_end("://");
    std::string::const_iterator prot_i = search(url_s.begin(), url_s.end(),
                                           prot_end.begin(), prot_end.end());
    protocol_.reserve(distance(url_s.begin(), prot_i));
    transform(url_s.begin(), prot_i,
              back_inserter(protocol_),
              &tolower); // protocol is icase
    if( prot_i == url_s.end() )
        return;
    advance(prot_i, prot_end.length());
    std::string::const_iterator path_i = find(prot_i, url_s.end(), '/');
    host_.reserve(distance(prot_i, path_i));
    transform(prot_i, path_i,
              back_inserter(host_),
              &tolower); // host is icase
    std::string::const_iterator query_i = find(path_i, url_s.end(), '?');
    path_.assign(path_i, query_i);
    if( query_i != url_s.end() )
        ++query_i;
    query_.assign(query_i, url_s.end());
}

// converts a request object to be a string
std::string reverse_proxy_request_handler::request_to_string(const request& req) {
    std::string proxy_request;
    std::string methods[] = {"GET", "POST", "PUT", "HEAD"};
    proxy_request.append(methods[req.method_] + " ");
    proxy_request.append(req.uri_ + " ");
    proxy_request.append(req.version_ + "\r\n");

    for (auto it = req.headers_.begin(); it != req.headers_.end(); it++) {
        proxy_request.append(it->first + ": " + it->second + "\r\n");
    }

    proxy_request.append(req.body_ + "\r\n");
    proxy_request.append("\r\n");
    return proxy_request;
}

// converts a proxy request or redirect request to point to the proper location.
request reverse_proxy_request_handler::get_proxy_request(const request& req, std::string uri) {
    request new_req;
    new_req.method_ = req.method_;
    new_req.version_ = req.version_;
    if (uri == "/") {
        new_req.uri_ = path_;
    } else {
         new_req.uri_ = path_ + uri;
    }
    new_req.headers_ = req.headers_;
    new_req.headers_["Connection"] = "close";
    new_req.headers_["Host"] = host_;
    new_req.body_ = req.body_;
    return new_req;
}

response reverse_proxy_request_handler::throw_400_error() {
    response_builder res;
    res.make_400_error();
    res.make_date_servername_headers();
    return res.get_response();
}