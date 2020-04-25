#include "session.h"
#include "response.h"
#include "connection.h"
#include "log_helper.h"

using boost::asio::ip::tcp;

session::session(std::unique_ptr<connection> connection, log_helper* log)
    : connection_(std::move(connection)), log_(log)
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

void session::process_req(size_t bytes_transferred)
{
    response response_;
    std::string request_str(data_.begin(), data_.begin() + bytes_transferred);
    echo_handler_.create_response(request_, request_str, response_);
    responses_.push_back(response_);
}

int session::num_responses() { return responses_.size(); }

void session::received_req(const boost::system::error_code& error,
                           size_t bytes_transferred)
{
    if (!error) {
        std::shared_ptr<session> shared_this(shared_from_this());

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
