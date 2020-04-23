#include "server.h"
#include <stdexcept>
#include "session.h"

using boost::asio::ip::tcp;

server::server(boost::asio::io_service& io_service, short port, log_helper* log)
    : acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
      signals_(io_service),
      socket_(tcp::socket(io_service)),
      io_(io_service),
      log_(log)
{
    signals_.add(SIGINT);
    signals_.add(SIGTERM);
    signals_.async_wait(boost::bind(&server::signal_handler, this,
                                    boost::asio::placeholders::error,
                                    boost::asio::placeholders::signal_number));
    start_accept();
}

void server::signal_handler(const boost::system::error_code& ec,
                            int signal_number)
{
    log_->log_warning_file("server is shutting down");
    acceptor_.close();
    io_.stop();
    throw std::runtime_error("Server Killed");  // Exit to main
}

void server::start_accept()
{
    std::unique_ptr<tcp_connection> conn =
        std::make_unique<tcp_connection>(std::move(socket_));
    std::shared_ptr<session> new_session =
        std::make_shared<session>(std::move(conn));

    acceptor_.async_accept(
        *(new_session->socket()),
        boost::bind(&server::handle_accept, this, new_session,
                    boost::asio::placeholders::error));
}

void server::handle_accept(std::shared_ptr<session> new_session,
                           const boost::system::error_code& error)
{
    if (!error) {
        new_session->start();
    }
    start_accept();
}
