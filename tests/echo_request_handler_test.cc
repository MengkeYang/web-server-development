#include "gtest/gtest.h"
#include <fstream>
#include <iostream>
#include "request.h"
#include "response.h"
#include "config_parser.h"
#include "echo_request_handler.h"
#include <string.h>


class echo_request_handler_test : public ::testing::Test
{
protected:
    void SetUp() override
    {
    }
    void TearDown() override { }
    NginxConfigParser config_parser;
    NginxConfig config;
};

TEST_F(echo_request_handler_test, echo_read)
{   
    config_parser.Parse("example_server_static", &config);
    //request
    request r;
    r.method_ = request::GET;
    r.http_version_ = "HTTP/1.1";
    r.uri_ = "/";
    r.body_ = "whole HTTP request";

    std::string expected = "GET / HTTP/1.1\r\n\r\nwhole HTTP request";

    echo_request_handler echo_handler;
    response result;
    result = echo_handler.handle_request(r);
    response_builder b;
    b.set_response(result);
    std::string response_body(reinterpret_cast<const char*>(boost::asio::buffer_cast<const unsigned char*>(b.build()[1])),
    expected.length());
    std::cout << response_body << std::endl;
    //output
    EXPECT_EQ(expected, response_body);
}
