#include "session.h"
#include "response.h"
#include "connection.h"
#include "log_helper.h"

#include <iostream>
using boost::asio::ip::tcp;

session::session(std::unique_ptr<connection> connection, log_helper* log,
                 const NginxConfig& config)
    : connection_(std::move(connection)), log_(log)
{
    std::map<std::string, std::string> uri_table_ = config.get_uri_table();
    for (std::pair<std::string, std::string> mapping : uri_table_) {
        if (mapping.second == "") {  // Location for echo
            std::unique_ptr<echo_request_handler> er =
                std::make_unique<echo_request_handler>();
            location_handlers_.insert(
                std::pair<std::string, std::unique_ptr<request_handler>>(
                    mapping.first, std::move(er)));
        } else {  // Location for serving static files
            std::unique_ptr<StaticRequestHandler> sr =
                std::make_unique<StaticRequestHandler>(mapping.second,
                                                       mapping.first);
            location_handlers_.insert(
                std::pair<std::string, std::unique_ptr<request_handler>>(
                    mapping.first, std::move(sr)));
        }
    }
}

tcp::socket* session::socket() { return connection_->socket(); }

void session::start()
{
    std::shared_ptr<session> shared_this(shared_from_this());
    connection_->read(boost::asio::buffer(data_, max_len),
                      static_cast<int>(max_len), &session::received_req,
                      shared_this);
}

void session::process_req(size_t bytes_transferred)
{
    response response_;
    // We will use the first match. TODO: use longest prefix.
    bool matched = false;
    for (auto&& pair : location_handlers_) {
        // We compare the prefix without the final "/". So /echo matches
        // with /echo/.
        if (request_.uri.length() > pair.first.length() &&
            request_.uri.compare(pair.first.length()-1, 1, "/") != 0)
            continue;
        if (request_.uri.compare(
                0, pair.first.length() - 1,
                pair.first.substr(0, pair.first.length() - 1)) == 0) {
            matched = true;
            response_ = {};
            pair.second->create_response(request_, response_);
            break;
        }
    }
    if (request_.parse_result == request_parser::good && !matched)
        response_.make_404_error();
    else if (!matched)
        response_.make_400_error();

    responses_.push_back(response_);
}

int session::num_responses() { return responses_.size(); }

void session::received_req(const boost::system::error_code& error,
                           size_t bytes_transferred)
{
    if (!error) {
        std::shared_ptr<session> shared_this(shared_from_this());
        request_ = {};  // Reset the request
        request_.raw_data = std::string(data_.data(), data_.data()+bytes_transferred);
        request_parser_.parse(request_, data_.data(),
                              data_.data() + bytes_transferred);

        log_->log_request_info(request_, connection_->socket());

        request_parser_.reset();

        process_req(bytes_transferred);

        connection_->write(responses_.back().build_response(),
                           &session::wait_for_req, shared_this);
    }
}

void session::wait_for_req(const boost::system::error_code& error)
{
    if (responses_.size() > 0) {
        responses_.pop_back();
    }
    if (!error) {
        std::shared_ptr<session> shared_this(shared_from_this());
        connection_->read(boost::asio::buffer(data_, max_len),
                          static_cast<int>(max_len), &session::received_req,
                          shared_this);
    }
}
