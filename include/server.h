// This server class create and delete session
#ifndef WNZA_SERVER_H_
#define WNZA_SERVER_H_
#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "config_parser.h"
#include "log_helper.h"
class session;

using boost::asio::ip::tcp;

class server
{
public:
    server(boost::asio::io_service& io_service, short port, const NginxConfig &config, log_helper *log);

private:
    // create a session object and socket, call handle_accept to process
    void start_accept();
    // call session::start to run, delete and build another session when encounter errors
    void handle_accept(session* new_session, const boost::system::error_code& error);
    void signal_handler(session* new_session,
                        const boost::system::error_code& ec, int signal_number);

    boost::asio::io_service& io_service_;
    std::shared_ptr<tcp::socket> socket_;
    tcp::acceptor acceptor_;
    boost::asio::signal_set signals_;
    log_helper* log_;
};
#endif  // WNZA_SERVER_H_
