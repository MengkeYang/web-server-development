#include "session.h"

using boost::asio::ip::tcp;

tcp::socket& session::socket()
{
    return socket_;
}

void session::start()
{
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
        boost::bind(&session::handle_read, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
}

void session::handle_read(const boost::system::error_code& error, size_t bytes_transferred) 
{
    if (!error) 
    {
	request_parser::result_type result;
        std::tie(result, std::ignore) = request_parser_.parse(
              request_, data_.data(), data_.data() + bytes_transferred);

	// std::cout<<result<<std::endl;

	if (result == request_parser::good)
        {
          char response[] = "200 OK\n";
	  request_parser_.reset();
          boost::asio::async_write(socket_, 
	    boost::asio::buffer(response, strlen(response)),
            boost::bind(&session::handle_write, this,
              boost::asio::placeholders::error));
        }
        else if (result == request_parser::bad)
        {
          char response[] = "400 BAD REQUEST\n";
	  request_parser_.reset();
          boost::asio::async_write(socket_, 
	    boost::asio::buffer(response, strlen(response)),
            boost::bind(&session::handle_write, this,
              boost::asio::placeholders::error));
        }
        else
        {
	  socket_.async_read_some(boost::asio::buffer(data_, max_length),
            boost::bind(&session::handle_read, this,
              boost::asio::placeholders::error,
              boost::asio::placeholders::bytes_transferred));
        }
	/*
        boost::asio::async_write(socket_, 
	    boost::asio::buffer(data_, bytes_transferred),
            boost::bind(&session::handle_write, this,
            boost::asio::placeholders::error));
	*/
    }
    else
    {
      delete this;
    }
}

void session::handle_write(const boost::system::error_code& error)
{
    if (!error)
    {
      socket_.async_read_some(boost::asio::buffer(data_, max_length),
          boost::bind(&session::handle_read, this,
              boost::asio::placeholders::error,
              boost::asio::placeholders::bytes_transferred));
    }
    else 
    {
      delete this;
    }
}
