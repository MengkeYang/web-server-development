#include "session.h"
#include <iostream>

using boost::asio::ip::tcp;

tcp::socket& session::socket() { return socket_; }

void session::start()
{
    socket_.async_read_some(
        boost::asio::buffer(data_, max_length),
        boost::bind(&session::handle_read, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
}

void session::handle_read(const boost::system::error_code& error,
                          size_t bytes_transferred)
{
    if (!error) {
        request_parser::result_type result;
        std::tie(result, std::ignore) = request_parser_.parse(
            request_, data_.data(), data_.data() + bytes_transferred);

        response response_;
        std::string request_str(data_.begin(), data_.begin() + bytes_transferred);
        response_.addResponse(result, request_str);
        responses_.push_back(response_);
//        boost::asio::const_buffer response_buf = response_.getResponse();

        boost::asio::async_write(socket_, responses_.back().getResponse(),
                                 boost::bind(&session::handle_write, this,
                                             boost::asio::placeholders::error));
    } else {
        delete this;
    }
}

void session::handle_write(const boost::system::error_code& error)
{
    if (responses_.size() > 0) {
        responses_.pop_back();
    }
    if (!error) {
        socket_.async_read_some(
            boost::asio::buffer(data_, max_length),
            boost::bind(&session::handle_read, this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
    } else {
        delete this;
    }
}
