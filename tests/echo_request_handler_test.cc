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
    echo_request_handler echo_handler;
    r.parse_result = request_parser::good;
    r.raw_data = "whole HTTP request";
    response result;
    echo_handler.create_response(r, result);
    std::vector<boost::asio::const_buffer> b = result.build_response();
    std::string response_body(reinterpret_cast<const char*>(boost::asio::buffer_cast<const unsigned char*>(b[1])),
    r.raw_data.length());
    //output
    EXPECT_EQ(r.raw_data, response_body);

}
