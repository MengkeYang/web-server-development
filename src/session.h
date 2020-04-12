// This session class handle read/write of tcp socket
#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "request.h"
#include "request_parser.h"

using boost::asio::ip::tcp;

class session
{
public:
    session(boost::asio::io_service& io_service) : socket_(io_service) {}
    // get tcp socket
    tcp::socket& socket();
    // listen tcp read socket and call handle_read to process
    void start();

private:
    // receive data from tcp read buffer
    // and call handle_write to process
    void handle_read(const boost::system::error_code& error, size_t bytes_transferred);
    // send data to socket write buffer and call handle_read to continue read data
    void handle_write(const boost::system::error_code& error);

    tcp::socket socket_;
    enum { max_length = 1024 };
    std::array<char, 8192> data_;
    // char data_[max_length];

    request request_;
    request_parser request_parser_;
};
