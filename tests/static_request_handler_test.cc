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
    NginxConfig* block_config = new NginxConfig;
    std::shared_ptr<NginxConfigStatement> statement(new NginxConfigStatement); 
    statement->tokens_.push_back("root");
    statement->tokens_.push_back("\".\"");
    block_config->statements_.push_back(statement);
    static_request_handler static_handler(block_config, "/static");
    r.method_ = request::method::GET;
    r.uri_ = "/static/random_test.txt";
    std::string rd("");
    response result;
    result = static_handler.handle_request(r);
    response_builder b;
    b.set_response(result);
    std::string response_body(reinterpret_cast<const char*>(boost::asio::buffer_cast<const unsigned char*>(b.build()[1])),
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
    NginxConfig* block_config = new NginxConfig;
    std::shared_ptr<NginxConfigStatement> statement(new NginxConfigStatement); 
    statement->tokens_.push_back("root");
    statement->tokens_.push_back("\".\"");
    block_config->statements_.push_back(statement);
    static_request_handler static_handler(block_config, "/static");
    r.method_ = request::method::GET;
    r.uri_ = "/static/cat.jpg";
    std::string rd("");
    response result;
    result = static_handler.handle_request(r);
    response_builder b;
    b.set_response(result);
    std::string response_body(reinterpret_cast<const char*>(boost::asio::buffer_cast<const unsigned char*>(b.build()[1])),
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
    NginxConfig* block_config = new NginxConfig;
    std::shared_ptr<NginxConfigStatement> statement(new NginxConfigStatement); 
    statement->tokens_.push_back("root");
    statement->tokens_.push_back("\".\"");
    block_config->statements_.push_back(statement);
    static_request_handler static_handler(block_config, "/static");
    r.method_ = request::method::GET;
    r.uri_ = "/static/cat.jpg";
    std::string rd("");
    response result;
    result = static_handler.handle_request(r);
    response_builder b;
    b.set_response(result);
    std::string response_body(reinterpret_cast<const char*>(boost::asio::buffer_cast<const unsigned char*>(b.build()[1])),
    expected.length());
    //output
    std::ofstream result_out;
    result_out.open ("static_test/result");
    result_out << response_body;
    result_out.close();
    EXPECT_EQ(expected, response_body);

}
