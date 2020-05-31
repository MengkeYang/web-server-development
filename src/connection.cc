#include "connection.h"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <memory>
#include <mutex>
#include <boost/date_time/posix_time/posix_time.hpp>

using boost::asio::ip::tcp;

tcp::socket* connection::socket()
{
    std::lock_guard<std::mutex> lk(lock_);
    return &socket_;
}

void tcp_connection::timeout(const boost::system::error_code& e) {
    if (e) return;
    socket()->close();
}

void tcp_connection::read(
    boost::asio::mutable_buffer buf, size_t maxlen,
    boost::function<void(std::shared_ptr<session> s,
                         const boost::system::error_code& error,
                         size_t bytes_transferred)>
        cb,
    std::shared_ptr<session> s)
{
    timer_.expires_from_now(boost::posix_time::seconds(3));
    timer_.async_wait(strand_.wrap(boost::bind(
        &tcp_connection::timeout, this, boost::asio::placeholders::error)));

    socket()->async_read_some(
        boost::asio::buffer(buf, maxlen),
        strand_.wrap(
            boost::bind(cb, s, boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred)));
}

void tcp_connection::write(
    std::shared_ptr<response_builder> res_build,
    boost::function<void(std::shared_ptr<session> s,
                         const boost::system::error_code& error,
                         std::shared_ptr<response_builder> res)>
        cb,
    std::shared_ptr<session> s)
{
    boost::asio::async_write(
        *socket(), res_build->build(),
        strand_.wrap(
            boost::bind(cb, s, boost::asio::placeholders::error, res_build)));
}
