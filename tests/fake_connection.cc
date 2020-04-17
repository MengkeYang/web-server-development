#include "connection.h"

using boost::asio::ip::tcp;

class fake_connection : public connection
{
    boost::system::error_code err_ =
        boost::system::errc::make_error_code(boost::system::errc::success);

public:
    int reads = 0;
    int writes = 0;
    fake_connection(boost::system::error_code &err,
                    std::shared_ptr<tcp::socket> sock)
        : connection(sock)
    {
        err_ = err;
    }
    void read(
        boost::asio::mutable_buffer buf, size_t maxlen,
        boost::function<void(session *s, const boost::system::error_code &error,
                             size_t bytes_transferred)>
            cb,
        session *s)
    {
        if (reads > 5) return;
        reads++;
        cb(s, err_, 0);
    }
    void write(std::vector<boost::asio::const_buffer> bufs,
               boost::function<void(session *s,
                                    const boost::system::error_code &error)>
                   cb,
               session *s)
    {
        writes++;
        cb(s, err_);
    }
};
