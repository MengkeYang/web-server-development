// This session class handle read/write of tcp socket
#ifndef WNZA_SESSION_H_
#define WNZA_SESSION_H_

#include <boost/asio.hpp>
#include "request.h"
#include "request_parser.h"
#include "echo_request_handler.h"
#include "static_request_handler.h"
#include "status_request_handler.h"
#include "not_found_request_handler.h"
#include "reverse_proxy_request_handler.h"
#include "connection.h"
class log_helper;
class response;

using boost::asio::ip::tcp;

class session : public std::enable_shared_from_this<session>
{
public:
    session(std::unique_ptr<connection> connection,
            std::map<std::string, std::unique_ptr<request_handler>>&
                location_handlers);
    // get tcp socket
    tcp::socket* socket();
    // listen tcp read socket and call handle_read to process
    void start();
    response_builder process_req(request request_);

private:
    // receive data from tcp read buffer
    // and call handle_write to process
    void received_req(const boost::system::error_code& error,
                      size_t bytes_transferred);
    // send data to socket write buffer and call handle_read to continue read
    // data
    void wait_for_req();
    void completed_request(const boost::system::error_code& error, std::shared_ptr<response_builder> res_build);

    enum { max_len = 8192 };
    std::array<char, max_len> data_;

    void thread_work(request r);
    std::unique_ptr<connection> connection_;
    int req_num = 0;  // Ranges from 0 - 9, so we support at most 10 requests
                      // being processsed at once
    request request_;
    request_parser request_parser_;
    log_helper& log_;
    std::map<std::string, std::unique_ptr<request_handler>>& location_handlers_;
};
#endif  // WNZA_SESSION_H_
