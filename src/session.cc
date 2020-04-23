#include "session.h"
#include <iostream>
#include "connection.h"

using boost::asio::ip::tcp;

session::session(std::shared_ptr<connection> connection)
{
    connection_ = connection;
}

std::shared_ptr<tcp::socket> session::socket() { return connection_->socket(); }

void session::start()
{
    connection_->read(boost::asio::buffer(data_, max_len),
                      static_cast<int>(max_len), &session::received_req, this);
}

void session::process_req(request_parser::result_type r,
                          size_t bytes_transferred)
{
    response response_;
    std::string request_str(data_.begin(), data_.begin() + bytes_transferred);
    response_.addResponse(r, request_str);
    responses_.push_back(response_);
}

int session::num_responses() { return responses_.size(); }

void session::received_req(const boost::system::error_code& error,
                           size_t bytes_transferred)
{
    if (!error) {
        request_parser::result_type result;
        std::tie(result, std::ignore) = request_parser_.parse(
            request_, data_.data(), data_.data() + bytes_transferred);
        request_parser_.reset();

        process_req(result, bytes_transferred);

        connection_->write(responses_.back().getResponse(),
                           &session::wait_for_req, this);
    } else {
        delete this;
    }
}

void session::wait_for_req(const boost::system::error_code& error)
{
    if (responses_.size() > 0) {
        responses_.pop_back();
    }
    if (!error) {
        connection_->read(boost::asio::buffer(data_, max_len),
                          static_cast<int>(max_len), &session::received_req,
                          this);
    } else {
        delete this;
    }
}
