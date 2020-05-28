#include "session.h"
#include "response.h"
#include "connection.h"
#include "request_parser.h"
#include "log_helper.h"
#include <boost/thread/thread.hpp>

using boost::asio::ip::tcp;

session::session(
    std::unique_ptr<connection> connection,
    std::map<std::string, std::unique_ptr<request_handler>>& location_handlers)
    : connection_(std::move(connection)),
      log_(log_helper::instance()),
      location_handlers_(location_handlers)
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

response_builder session::process_req(const request& request)
{
    response response;
    response_builder res_build;
    bool matched = false;
    int max_matched_length = 0;
    std::string max_matched_key;
    if (request.method_ != request::method::INVALID) {
        for (auto&& pair : location_handlers_) {
            // We compare the prefix without the final "/". So /echo matches
            // with /echo/.
            if (request.uri_.length() > pair.first.length() &&
                request.uri_.compare(pair.first.length() - 1, 1, "/") != 0)
                continue;
            if (request.uri_.compare(
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
            location_handlers_.at(max_matched_key)->handle_request(request);
        res_build.set_response(response);
        log_.log_metrics(
            request, response, connection_.get(),
            location_handlers_.at(max_matched_key)->get_handler_name());
    } else
        res_build.make_400_error();

    return res_build;
}

void session::thread_work(const request& r)
{
    std::shared_ptr<response_builder> res_build =
        std::make_shared<response_builder>();
    std::shared_ptr<session> shared_this(shared_from_this());

    *res_build = process_req(r);

    connection_->write(res_build, &session::completed_request, shared_this);
}

void session::received_req(const boost::system::error_code& error,
                           size_t bytes_transferred)
{
    if (!error) {
        std::vector<request> requests;
        std::shared_ptr<session> shared_this(shared_from_this());
        request_parser::result_type rt;
        char* body_start;
        int body_len;

        // Read until the end of the available data, could be multiple
        // requests. An incomplete request is stored in request_ until the next
        // read.
        do {
            std::tie(rt, body_start, body_len) = request_parser_.parse(
                request_, data_.data(), data_.data() + bytes_transferred);
            if (rt != request_parser::result_type::indeterminate) {
                requests.push_back(request_);
                request_ = {};
                request_parser_.reset();
                std::tie(rt, body_start, body_len) =
                    request_parser_.parse(request_, body_start + body_len,
                                          data_.data() + bytes_transferred);
            }
        } while (body_start + body_len < data_.data() + bytes_transferred);

        for (int i = 0; i < requests.size(); i++) {
            log_.log_request_info(requests[i], connection_.get());
            thread_work(requests[i]);
        }
        wait_for_req();
    }
}

void session::completed_request(const boost::system::error_code& error,
                                std::shared_ptr<response_builder> res_build)
{
    log_.log_response_sent();
}

void session::wait_for_req()
{
    std::shared_ptr<session> shared_this(shared_from_this());
    connection_->read(boost::asio::buffer(data_, max_len),
                      static_cast<int>(max_len), &session::received_req,
                      shared_this);
}
