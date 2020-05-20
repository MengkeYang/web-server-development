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
#include "request_handler.h"


using boost::asio::ip::tcp;

class session_test : public ::testing::Test
{
protected:
    session_test()
    {
        config_parser.Parse("test_server_config", &config);

        std::vector<location_parse_result> location_results = config.get_location_result();
        for (location_parse_result loc_res : location_results) {
            if (loc_res.handler_name == "EchoHandler") {  // Location for echo
                std::unique_ptr<echo_request_handler> er =
                    std::make_unique<echo_request_handler>();
                location_handlers_.insert(
                    std::pair<std::string, std::unique_ptr<request_handler>>(
                        loc_res.uri, std::move(er)));
            } else if (loc_res.handler_name == "StaticHandler"){  // Location for serving static files
                std::unique_ptr<static_request_handler> sr =
                    std::make_unique<static_request_handler>(loc_res.block_config, loc_res.uri);
                location_handlers_.insert(
                    std::pair<std::string, std::unique_ptr<request_handler>>(
                        loc_res.uri, std::move(sr)));
            }
        }
    }
    boost::system::error_code no_error =
        boost::system::errc::make_error_code(boost::system::errc::success);
    NginxConfigParser config_parser;
    NginxConfig config;
    boost::asio::io_service io;
    tcp::socket s = tcp::socket(io);

    std::shared_ptr<response_builder> res_build = std::make_shared<response_builder>();

    // initialize location_handler
    std::map<std::string, std::unique_ptr<request_handler>> location_handlers_;
};

TEST_F(session_test, bad_parse_generates_response)
{
    std::unique_ptr<fake_connection> conn =
        std::make_unique<fake_connection>(no_error, std::move(s), res_build);
    session session_(std::move(conn), location_handlers_);
    request req;
    response_builder rb = session_.process_req(req);
    response r = rb.get_response();
    EXPECT_EQ(r.code_, response::status_code::BAD_REQ);
}

TEST_F(session_test, good_parse_generates_ok_code)
{
    std::unique_ptr<fake_connection> conn =
        std::make_unique<fake_connection>(no_error, std::move(s), res_build);
    session session_(std::move(conn), location_handlers_);
    request req;
    req.uri_ = "/echo/";
    req.version_ = "HTTP/1.1";
    req.method_ = request::method::GET;

    response_builder rb = session_.process_req(req);
    response r = rb.get_response();
    EXPECT_EQ(r.code_, response::status_code::OK);
}
