#include <fstream>
#include <memory>
#include <streambuf>
#include <string>
#include "connection.h"
#include "fake_connection.cc"
#include "log_helper.h"
#include "request_parser.h"
#include "session.h"
#include "response.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "config_parser.h"


using boost::asio::ip::tcp;

class MockLogHelper : public log_helper
{
public:
    MockLogHelper() {}
    void init() {}
    void log_request_info(request req, tcp::socket* socket) {}
    void log_trace_file(std::string trace_message) {}
    void log_error_file(std::string error_message) {}
    void log_warning_file(std::string warning_message) {}
};

class session_test : public ::testing::Test
{
protected:
    session_test()
    {
        config_parser.Parse("example_config", &config);
    }
    boost::system::error_code no_error =
        boost::system::errc::make_error_code(boost::system::errc::success);
    boost::system::error_code error = boost::system::errc::make_error_code(
        boost::system::errc::not_supported);
    boost::asio::io_service io;
    tcp::socket s = tcp::socket(io);
    MockLogHelper* lg = new MockLogHelper();
    NginxConfigParser config_parser;
    NginxConfig config;
    
};

TEST_F(session_test, bad_parse_generates_response)
{
    std::unique_ptr<fake_connection> conn =
        std::make_unique<fake_connection>(no_error, std::move(s));
    session session_(std::move(conn), lg, config);
    session_.process_req(0);
    EXPECT_EQ(session_.num_responses(), 1);
}

TEST_F(session_test, return_socket)
{
    std::unique_ptr<fake_connection> conn =
        std::make_unique<fake_connection>(no_error, std::move(s));
    session session_(std::move(conn), lg, config);
    auto sock = session_.socket();
    EXPECT_NE(nullptr, sock);
}

TEST_F(session_test, responses_are_cleared_once_sent)
{
    std::unique_ptr<fake_connection> conn =
        std::make_unique<fake_connection>(no_error, std::move(s));
    std::shared_ptr<session> session_ =
        std::make_shared<session>(std::move(conn), lg, config);
    session_->start();
    EXPECT_EQ(session_->num_responses(), 0);
}

TEST_F(session_test, session_delete_no_segfault)
{
    std::unique_ptr<fake_connection> conn =
        std::make_unique<fake_connection>(error, std::move(s));
    std::shared_ptr<session> session_ =
        std::make_shared<session>(std::move(conn), lg, config);
    EXPECT_NO_THROW(session_->start());
}
