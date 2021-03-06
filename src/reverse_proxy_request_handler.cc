#include <iostream>
#include "string.h"
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/algorithm/string/replace.hpp>
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
#include "log_helper.h"

using boost::asio::buffers_begin;
using boost::asio::ip::tcp;

reverse_proxy_request_handler::reverse_proxy_request_handler(
    const NginxConfig* config, const std::string& prefix_uri)
{
    // Ensure the handler's URI prefix always ends in "/"
    int plen = prefix_uri.size();
    if (prefix_uri.compare(plen - 1, 1, "/") != 0)
        prefix_ = prefix_uri + "/";
    else
        prefix_ = prefix_uri;

    // Ensure the handler's destination always ends in "/"
    dest_ = config->get_value_from_statement("dest");
    dest_ = dest_.substr(1, dest_.length() - 2);
    int dlen = dest_.size();
    if (dest_.compare(dlen - 1, 1, "/") != 0) dest_ += "/";

    // get the port number from the config block, or assign a default port
    // value. This port number is used for all connections, even redirects
    if (!config->get_value_from_statement("port").empty())
        port_ = config->get_value_from_statement("port");
    else
        port_ = "80";

    // parse out the parts of our specified destination
    // host_ is the destination which will appear under the "Host:" header.
    // path_ is everthing before the last / in the requested URI. If
    //      /ucla/admission is requested, then /ucla is the path_ and /admission
    //      will be the query
    url loc;
    loc = parse_url(dest_);
    host_ = loc.host_;
    path_ = loc.path_;
}

request_handler* reverse_proxy_request_handler::init(
    const std::string& prefix_uri, const NginxConfig& config)
{
    if (!config.get_value_from_statement("dest").empty() &&
        !prefix_uri.empty()) {
        return new reverse_proxy_request_handler(&config, prefix_uri);
    } else {
        return not_found_request_handler::init(prefix_uri, config);
    }
}

response reverse_proxy_request_handler::handle_request(const request& req)
{
    log_helper& log = log_helper::instance();
    // verify that the host and path we're about to contact exist
    if (host_ == "" || path_ == "") {
        log.log_error_file("Destination: " + dest_ + " is malformed");
        return throw_400_error();
    }

    // Make sure request uri ends in /
    int ulen = req.uri_.size();
    std::string uri = req.uri_;
    if (req.uri_.compare(ulen - 1, 1, "/") != 0) uri += "/";

    // Find the handler's prefix in the uri
    size_t index = uri.find(prefix_);
    if (index == std::string::npos) {
        // No match
        log.log_error_file("Proxy handler received uri: " + uri +
                           " but this handler is for path: " + prefix_);
        response_builder res_build;
        res_build.make_404_error();
        return res_build.get_response();
    } else {
        std::string new_uri = uri.substr(index + prefix_.size() - 1);
        if (new_uri != "/")
            new_uri.pop_back();  // Remove the trailing '/' from all URIs
        url loc;
        loc.host_ = host_;
        loc.path_ = new_uri;
        request new_req = get_proxy_request(req, loc);
        return send_request(new_req, host_);
    }
}

// for reference:
// https://www.boost.org/doc/libs/1_50_0/doc/html/boost_asio/example/http/client/sync_client.cpp
// req: the request object which should be sent to host "host".
// host: host to send the request to. The uri is specified by req.uri_.
response reverse_proxy_request_handler::send_request(const request& req,
                                                     std::string host)
{
    log_helper& log = log_helper::instance();
    response_builder res;
    // obtain the request as a string
    std::string curr_request = request_to_string(req);
    // an instance of the io service to send the request
    boost::asio::io_service io_service;
    boost::system::error_code ec;

    // get a list of endpoints corresponding to the server name.
    tcp::resolver resolver(io_service);
    tcp::resolver::query query(host, port_);
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query, ec);
    if (ec) {
        log.log_error_file("Could not resolve endpoint for host: " + host);
        return throw_400_error();
    }

    // try each endpoint until we successfully establish a connection.
    tcp::socket socket(io_service);
    boost::asio::connect(socket, endpoint_iterator, ec);
    if (ec) {
        log.log_error_file("Could not connect to host: " + host);
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
    std::getline(response_stream,
                 status_message);  // contains a space in front of it!
    if (!response_stream || http_version.substr(0, 5) != "HTTP/") {
        log.log_error_file("No HTTP response detected from server");
        return throw_400_error();
    }

    std::string header;
    // handle redirection
    if (status_code == 302 || status_code == 301) {
        while (std::getline(response_stream, header) && header != "\r") {
            if (header.find("Location") != std::string::npos) {
                header.pop_back();
                std::string location = header.substr(header.find(":") + 2);
                url prev_loc;
                prev_loc.host_ = host;
                prev_loc.path_ = req.uri_;
                return handle_redirect(req, location, prev_loc);
            }
        }
        log.log_error_file("No redirect location was provided");
        return throw_400_error();
    } else if (status_code == 200) {
        res.set_code(response::status_code::OK);
    } else if (status_code == 404) {
        res.make_404_error();
        res.make_date_servername_headers();
        return res.get_response();
    } else {
        log.log_error_file("Unsupported return code: " +
                           std::to_string(status_code));
        return throw_400_error();
    }

    // read the response headers, which are terminated by a blank line
    boost::asio::read_until(socket, response_buffer, "\r\n\r\n", ec);

    // process the response headers. A line with contain "name: value\r"
    while (std::getline(response_stream, header) && header != "\r") {
        header.pop_back();  // Remove the trailing "\r"
        std::string token = header.substr(0, header.find(":"));
        if (token != "Transfer-Encoding") {
            res.add_header(token, header.substr(header.find(":") + 2));
        }
    }

    // write whatever content we already have to the body.
    std::string body;
    std::string content(std::istreambuf_iterator<char>(response_stream), {});
    body += content;

    // read until EOF, writing data to a content string so we can finish off our
    // body.
    boost::system::error_code error;
    content = "";
    while (boost::asio::read(socket, response_buffer,
                             boost::asio::transfer_at_least(1), error)) {
        auto bufs = response_buffer.data();
        std::string content_string(
            buffers_begin(bufs), buffers_begin(bufs) + response_buffer.size());
        content = content_string;
    }
    body += content;

    if (error != boost::asio::error::eof)
        throw boost::system::system_error(error);

    // add on final content-length header, body, and final fields to prepare the
    // response.
    boost::replace_all(body, "href=\"/", "href=\"" + prefix_);
    boost::replace_all(body, "src=\"/", "src=\"" + prefix_);
    res.add_header("Content-Length", std::to_string(body.length()));
    res.add_body(body);
    res.make_date_servername_headers();
    return res.get_response();
}

// create the new redirect request object
response reverse_proxy_request_handler::handle_redirect(const request& req,
                                                        std::string location,
                                                        url prev_loc)
{
    log_helper& log = log_helper::instance();
    url redir_loc;

    // format the location so it's ready for the parser, obtain the new params
    // of the location
    if (location.find("http://") != std::string::npos ||
        location.find("https://") != std::string::npos) {
        redir_loc = parse_url(location);
    } else {
        location = "http://" + location;
        redir_loc = parse_url(location);
    }

    if (redir_loc.host_ == prev_loc.host_ &&
        redir_loc.path_ == prev_loc.path_) {
        log.log_error_file("Infinite redirection to: " + prev_loc.host_ + "/" +
                           prev_loc.path_);
        return throw_400_error();
    }
    // obtain the new request and send it
    request new_req = get_proxy_request(req, redir_loc);
    response r = send_request(new_req, redir_loc.host_);
    return r;
}

// referenced from
// https://stackoverflow.com/questions/2616011/easy-way-to-parse-a-url-in-c-cross-platform
// Returns url struct containing: host_ which is the host to make connections to
//                                path_ which is the prefix of the uri a "?"
//                                query_ which is the suffix of the uri after
//                                any "?"
url reverse_proxy_request_handler::parse_url(const std::string& url_s)
{
    url parsed;
    std::string protocol;
    log_helper& log = log_helper::instance();
    const std::string prot_end("://");
    std::string::const_iterator prot_i =
        search(url_s.begin(), url_s.end(), prot_end.begin(), prot_end.end());
    protocol.reserve(distance(url_s.begin(), prot_i));
    transform(url_s.begin(), prot_i, back_inserter(protocol),
              &tolower);  // protocol is icase
    if (prot_i == url_s.end()) return parsed;
    advance(prot_i, prot_end.length());
    std::string::const_iterator path_i = find(prot_i, url_s.end(), '/');
    parsed.host_.reserve(distance(prot_i, path_i));
    transform(prot_i, path_i, back_inserter(parsed.host_),
              &tolower);  // host is icase
    std::string::const_iterator query_i = find(path_i, url_s.end(), '?');
    parsed.path_.assign(path_i, query_i);
    if (query_i != url_s.end()) ++query_i;
    parsed.query_.assign(query_i, url_s.end());
    return parsed;
}

// converts a request object to be a string
std::string reverse_proxy_request_handler::request_to_string(const request& req)
{
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
request reverse_proxy_request_handler::get_proxy_request(const request& req,
                                                         url dest)
{
    request new_req;
    new_req.method_ = req.method_;
    new_req.version_ = "HTTP/1.0";
    new_req.uri_ = dest.path_ + dest.query_;

    new_req.headers_ = req.headers_;
    new_req.headers_.erase("Connection");
    new_req.headers_["Accept-Encoding"] = "*/*";
    new_req.headers_["Host"] = dest.host_;
    new_req.body_ = req.body_;
    return new_req;
}

response reverse_proxy_request_handler::throw_400_error()
{
    response_builder res;
    res.make_400_error();
    res.make_date_servername_headers();
    return res.get_response();
}

std::string reverse_proxy_request_handler::get_handler_name(){
    return "ProxyHandler";
}
