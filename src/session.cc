#include "session.h"
#include <iostream>
#include "connection.h"

using boost::asio::ip::tcp;

session::session(std::unique_ptr<connection> connection)
    : connection_(std::move(connection))
{
}

tcp::socket* session::socket() { return connection_->socket(); }

void session::start()
{
    std::shared_ptr<session> shared_this(shared_from_this());
    connection_->read(boost::asio::buffer(data_, max_len),
                      static_cast<int>(max_len), &session::received_req,
                      shared_this);
}

void session::process_req(request_parser::result_type r,
                          size_t bytes_transferred)
{
    response response_;
    if (r == request_parser::good) {
        std::string request_str(data_.begin(),
                                data_.begin() + bytes_transferred);
        response_.set_status("200 OK");
        response_.add_header("Content-Length",
                             std::to_string(bytes_transferred));
        response_.add_header("Content-Type", "text/plain");
        response_.add_data(request_str);
    } else
        response_.set_status("400 Bad Request");
    responses_.push_back(response_);
}

int session::num_responses() { return responses_.size(); }

void session::received_req(const boost::system::error_code& error,
                           size_t bytes_transferred)
{
    if (!error) {
        std::shared_ptr<session> shared_this(shared_from_this());

        request_parser::result_type result;
        std::tie(result, std::ignore) = request_parser_.parse(
            request_, data_.data(), data_.data() + bytes_transferred);
        request_parser_.reset();

        process_req(result, bytes_transferred);

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
