// This server class create and delete session
#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "config_parser.h"

class session;

using boost::asio::ip::tcp;

class server
{
public:
    server(boost::asio::io_service& io_service, short port, const NginxConfig &config);

private:
    // create a session object and socket, call handle_accept to process
    void start_accept();
    // call session::start to run, delete and build another session when encounter errors
    void handle_accept(session* new_session, const boost::system::error_code& error);

    boost::asio::io_service& io_service_;
    tcp::acceptor acceptor_;
};
