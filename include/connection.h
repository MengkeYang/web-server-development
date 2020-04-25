#ifndef WNZA_CONNECTION_H_
#define WNZA_CONNECTION_H_

#include <boost/asio.hpp>
#include <boost/function.hpp>

using boost::asio::ip::tcp;

class session;

/**
 * Interface for an asynchronous connection which uses a socket
 *
 * A connection holds a socket which can be read and written to by using the
 * read and write functions. To construct a connection, a shared_ptr to a
 * socket must be provided and this socket pointer can be accessed through the
 * socket() method. The read and write functions are both passed callback
 * functions cb which are called once a read or write is completed
 * respectively, and the resulting error codes are passed to cb. read
 * additionally passed the number of bytes transferred to the callback cb.
 */
class connection
{
    tcp::socket socket_;

public:
    connection(tcp::socket socket) : socket_(std::move(socket)) {}
    tcp::socket *socket();
    virtual void read(
        boost::asio::mutable_buffer buf, size_t maxlen,
        boost::function<void(std::shared_ptr<session> s,
                             const boost::system::error_code &error,
                             size_t bytes_transferred)>
            cb,
        std::shared_ptr<session> s) = 0;
    virtual void write(
        std::vector<boost::asio::const_buffer> bufs,
        boost::function<void(std::shared_ptr<session> s,
                             const boost::system::error_code &error)>
            cb,
        std::shared_ptr<session> s) = 0;
    virtual ~connection() { socket_.close(); }
};

/**
 * Implementation of an asynchronous TCP connection
 *
 * read and write are implemented to use TCP which guarantees reliable delivery
 * of the data.
 */
class tcp_connection : public connection
{
public:
    tcp_connection(tcp::socket s) : connection(std::move(s)) {}
    void read(boost::asio::mutable_buffer buf, size_t maxlen,
              boost::function<void(std::shared_ptr<session> s,
                                   const boost::system::error_code &error,
                                   size_t bytes_transferred)>
                  cb,
              std::shared_ptr<session> s);
    void write(std::vector<boost::asio::const_buffer> bufs,
               boost::function<void(std::shared_ptr<session> s,
                                    const boost::system::error_code &error)>
                   cb,
               std::shared_ptr<session> s);
};
#endif  // WNZA_CONNECTION_H_
