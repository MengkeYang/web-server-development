#include "session.h"
#include "response.h"
#include "connection.h"
#include "request_parser.h"
#include "log_helper.h"

#include <iostream>
using boost::asio::ip::tcp;

session::session(
    std::unique_ptr<connection> connection,
    std::map<std::string, std::unique_ptr<request_handler>>& location_handlers)
    : connection_(std::move(connection)),
      log_(log_helper::instance()),
      location_handlers_(location_handlers)
{}

tcp::socket* session::socket() { return connection_->socket(); }

void session::start()
{
    std::shared_ptr<session> shared_this(shared_from_this());
    connection_->read(boost::asio::buffer(data_, max_len),
                      static_cast<int>(max_len), &session::received_req,
                      shared_this);
}

response_builder session::process_req(size_t bytes_transferred)
{
    response response;
    response_builder res_build;
    bool matched = false;
    int max_matched_length = 0;
    std::string max_matched_key;
    if (request_.method_ != request::method::INVALID) {
        for (auto&& pair : location_handlers_) {
            // We compare the prefix without the final "/". So /echo matches
            // with /echo/.
            if (request_.uri_.length() > pair.first.length() &&
                request_.uri_.compare(pair.first.length() - 1, 1, "/") != 0)
                continue;
            if (request_.uri_.compare(
                    0, pair.first.length() - 1,
                    pair.first.substr(0, pair.first.length() - 1)) == 0) {
                if (max_matched_length < pair.first.length()) {
                    max_matched_length = pair.first.length();
                    max_matched_key = pair.first;
                }
                matched = true;
            }
        }
    }
    if (matched) {
        response =
            location_handlers_.at(max_matched_key)->handle_request(request_);
        res_build.set_response(response);
    }
    else
        res_build.make_400_error();

    return res_build;
}

void session::received_req(const boost::system::error_code& error,
                           size_t bytes_transferred)
{
    if (!error) {
        std::shared_ptr<session> shared_this(shared_from_this());
        request_ = {};  // Reset the request
        request_parser_.parse(request_, data_.data(),
                              data_.data() + bytes_transferred);

        log_.log_request_info(request_, connection_->socket());

        response_builder res_build;
        res_build = process_req(bytes_transferred);
        response resp = res_build.get_response();
        log_.log_response_info(request_, resp, connection_->socket());
        connection_->write(res_build, &session::wait_for_req,
                           shared_this);
    }
}

void session::wait_for_req(const boost::system::error_code& error, response_builder res_build)
{
    if (!error) {
        std::shared_ptr<session> shared_this(shared_from_this());
        connection_->read(boost::asio::buffer(data_, max_len),
                          static_cast<int>(max_len), &session::received_req,
                          shared_this);
    }
}
