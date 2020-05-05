#include "gtest/gtest.h"
#include <fstream>
#include <iostream>
#include "response.h"
#include "config_parser.h"
#include "static_request_handler.h"
#include <string.h>


class static_request_handler_test : public ::testing::Test
{
protected:
    void SetUp() override
    {
    }
    void TearDown() override { }
    std::string expected;
    NginxConfigParser config_parser;
    NginxConfig config;
};

TEST_F(static_request_handler_test, file_read)
{   
    //set up
    std::string file_name = "random_test.txt";
    std::ifstream file(file_name, std::ios::binary);
    std::string body;
    char c;
    while (file.get(c))
        body += c;
    file.close();
    expected = body;
    std::ofstream expected_out;
    expected_out.open ("static_test/expected");
    expected_out << expected;
    expected_out.close();

    config_parser.Parse("example_config_static", &config);

    //request
    request r;
    static_request_handler static_handler(".", "/static");
    r.parse_result = request_parser::good;
    r.uri = "/static/random_test.txt";
    std::string rd("");
    response result;
    static_handler.create_response(r, result);
    std::vector<boost::asio::const_buffer> b = result.build_response();
    std::string response_body(reinterpret_cast<const char*>(boost::asio::buffer_cast<const unsigned char*>(b[1])),
    expected.length());
    //output
    std::ofstream result_out;
    result_out.open ("static_test/result");
    result_out << response_body;
    result_out.close();
    EXPECT_EQ(expected, response_body);

}

TEST_F(static_request_handler_test, image_read)
{   
    //set up
    std::string file_name = "cat.jpg";
    std::ifstream file(file_name, std::ios::binary);
    std::string body;
    char c;
    while (file.get(c))
        body += c;
    file.close();
    expected = body;
    std::ofstream expected_out;
    expected_out.open ("static_test/expected");
    expected_out << expected;
    expected_out.close();

    config_parser.Parse("example_config_static", &config);

    //request
    request r;
    static_request_handler static_handler(".", "/static");
    r.parse_result = request_parser::good;
    r.uri = "/static/cat.jpg";
    std::string rd("");
    response result;
    static_handler.create_response(r, result);
    std::vector<boost::asio::const_buffer> b = result.build_response();
    std::string response_body(reinterpret_cast<const char*>(boost::asio::buffer_cast<const unsigned char*>(b[1])),
    expected.length());
    //output
    std::ofstream result_out;
    result_out.open ("static_test/result");
    result_out << response_body;
    result_out.close();
    EXPECT_EQ(expected, response_body);

}

TEST_F(static_request_handler_test, diff_config_file)
{   
    //set up
    std::string file_name = "cat.jpg";
    std::ifstream file(file_name, std::ios::binary);
    std::string body;
    char c;
    while (file.get(c))
        body += c;
    file.close();
    expected = body;
    std::ofstream expected_out;
    expected_out.open ("static_test/expected");
    expected_out << expected;
    expected_out.close();

    config_parser.Parse("./example_config_static2", &config);

    //request
    request r;
    static_request_handler static_handler(".", "/static");
    r.parse_result = request_parser::good;
    r.uri = "/static/cat.jpg";
    std::string rd("");
    response result;
    static_handler.create_response(r, result);
    std::vector<boost::asio::const_buffer> b = result.build_response();
    std::string response_body(reinterpret_cast<const char*>(boost::asio::buffer_cast<const unsigned char*>(b[1])),
    expected.length());
    //output
    std::ofstream result_out;
    result_out.open ("static_test/result");
    result_out << response_body;
    result_out.close();
    EXPECT_EQ(expected, response_body);

}