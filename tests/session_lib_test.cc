#include <fstream>
#include <memory>
#include <streambuf>
#include <string>
#include "connection.h"
#include "gtest/gtest.h"
#include "request_parser.h"
#include "session.h"
#include "fake_connection.cc"

using boost::asio::ip::tcp;

class session_test : public ::testing::Test
{
protected:
    boost::system::error_code no_error =
        boost::system::errc::make_error_code(boost::system::errc::success);
    boost::system::error_code error = boost::system::errc::make_error_code(
        boost::system::errc::not_supported);
    boost::asio::io_service io;
    tcp::socket s = tcp::socket(io);
};

TEST_F(session_test, bad_parse_generates_response)
{
    std::unique_ptr<fake_connection> conn =
        std::make_unique<fake_connection>(no_error, std::move(s));
    session session_(std::move(conn));
    session_.process_req(request_parser::result_type::bad, 0);
    EXPECT_EQ(session_.num_responses(), 1);
}

TEST_F(session_test, return_socket)
{
    std::unique_ptr<fake_connection> conn =
        std::make_unique<fake_connection>(no_error, std::move(s));
    session session_(std::move(conn));
    auto sock = session_.socket();
    EXPECT_NE(nullptr, sock);
}

TEST_F(session_test, responses_are_cleared_once_sent)
{
    std::unique_ptr<fake_connection> conn =
        std::make_unique<fake_connection>(no_error, std::move(s));
    std::shared_ptr<session> session_ = std::make_shared<session>(std::move(conn));
    session_->start();
    EXPECT_EQ(session_->num_responses(), 0);
}

TEST_F(session_test, session_delete_no_segfault)
{
    std::unique_ptr<fake_connection> conn =
        std::make_unique<fake_connection>(error, std::move(s));
    std::shared_ptr<session> session_ = std::make_shared<session>(std::move(conn));
    EXPECT_NO_THROW(session_->start());
}
