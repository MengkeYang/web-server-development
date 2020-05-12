#include "connection.h"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <memory>

using boost::asio::ip::tcp;

tcp::socket* connection::socket() { return &socket_; }

void tcp_connection::read(
    boost::asio::mutable_buffer buf, size_t maxlen,
    boost::function<void(std::shared_ptr<session> s,
                         const boost::system::error_code& error,
                         size_t bytes_transferred)>
        cb,
    std::shared_ptr<session> s)
{
    socket()->async_read_some(
        boost::asio::buffer(buf, maxlen),
        boost::bind(cb, s, boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
}

void tcp_connection::write(
    response_builder res_build,
    boost::function<void(std::shared_ptr<session> s,
                         const boost::system::error_code& error,
                         response_builder res)>
        cb,
    std::shared_ptr<session> s)
{
    boost::asio::async_write(
        *socket(), res_build.build(),
        boost::bind(cb, s, boost::asio::placeholders::error, res_build));
}
