#include <memory>
#include <boost/asio.hpp>
#include "connection.h"

using boost::asio::ip::tcp;

std::shared_ptr<tcp::socket> connection::socket() { return socket_; }

void tcp_connection::read(
    boost::asio::mutable_buffer buf, size_t maxlen,
    boost::function<void(session* s, const boost::system::error_code& error,
                         size_t bytes_transferred)>
        cb,
    session* s)
{
    socket()->async_read_some(
        boost::asio::buffer(buf, maxlen),
        boost::bind(cb, s, boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
}

void tcp_connection::write(
    std::vector<boost::asio::const_buffer> bufs,
    boost::function<void(session* s, const boost::system::error_code& error)>
        cb,
    session* s)
{
    boost::asio::async_write(
        *socket(), bufs, boost::bind(cb, s, boost::asio::placeholders::error));
}