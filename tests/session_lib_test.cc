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
    std::shared_ptr<tcp::socket> s =
        std::make_shared<tcp::socket>(tcp::socket(io));
};

TEST_F(session_test, bad_parse_generates_response)
{
    std::shared_ptr<fake_connection> conn =
        std::make_shared<fake_connection>(no_error, s);
    session session_(conn);
    session_.process_req(request_parser::result_type::bad, 0);
    EXPECT_EQ(session_.num_responses(), 1);
}

TEST_F(session_test, return_socket)
{
    std::shared_ptr<fake_connection> conn =
        std::make_shared<fake_connection>(no_error, s);
    session session_(conn);
    auto sock = session_.socket();
    EXPECT_EQ(s, sock);
}

TEST_F(session_test, responses_are_cleared_once_sent)
{
    std::shared_ptr<fake_connection> conn =
        std::make_shared<fake_connection>(no_error, s);
    session session_(conn);
    session_.start();
    EXPECT_EQ(session_.num_responses(), 0);
}
