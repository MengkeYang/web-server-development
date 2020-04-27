#include "session.h"
#include "response.h"
#include "connection.h"
#include "log_helper.h"

using boost::asio::ip::tcp;

session::session(std::unique_ptr<connection> connection, log_helper* log,
                 const NginxConfig& config)
    : connection_(std::move(connection)), log_(log), static_handler_(config)
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
    std::string request_str(data_.begin(), data_.begin() + bytes_transferred);
    response response_;
    if (request_.parse_result == request_parser::good) {
        // look at request.url, and check
        std::string urlstr = request_.uri;
        std::cerr << urlstr.substr(1, urlstr.find("/", 1)) << std::endl;
        if (urlstr.substr(1, urlstr.find("/", 1)) == "echo")
            echo_handler_.create_response(request_, request_str, response_);
        else if (urlstr.substr(1, urlstr.find("/", 1)) == "static/")
            static_handler_.create_response(request_, request_str, response_);
    } else {
        // The request was malformed, so echo it back
        log_->log_warning_file("Received unparsable request.");
        echo_handler_.create_response(request_, request_str, response_);
    }
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
