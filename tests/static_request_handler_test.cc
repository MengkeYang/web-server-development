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
    response get_response(request& req, std::string root) {
        response result;
        NginxConfig* block_config = new NginxConfig;
        std::shared_ptr<NginxConfigStatement> statement(new NginxConfigStatement);

        statement->tokens_.push_back("root");
        statement->tokens_.push_back(root);
        block_config->statements_.push_back(statement);
        static_request_handler static_handler(block_config, "/static");

        result = static_handler.handle_request(req);
        return result;
    }
};

// We will get the 404 handler due to faulty config
TEST_F(static_request_handler_test, 404_response_no_root_provided)
{
    request r;
    NginxConfig* block_config = new NginxConfig;
    std::shared_ptr<NginxConfigStatement> statement(new NginxConfigStatement);
    statement->tokens_.push_back("moot");
    statement->tokens_.push_back("\".\"");
    block_config->statements_.push_back(statement);

    request_handler* rh = static_request_handler::init("/static", *block_config);

    r.method_ = request::method::GET;
    r.uri_ = "/static/cat.jpg";
    response result;
    result = rh->handle_request(r);
    response_builder b;
    bool check = (result.code_ == response::status_code::NOT_FOUND);
    EXPECT_EQ(check, true);
}

TEST_F(static_request_handler_test, 400_response_bad_request)
{
    request r;
    r.method_ = request::method::GET;
    r.uri_ = "/static/cat.jpg";

    response result;
    result = get_response(r, "\".\"");

    EXPECT_EQ(result.code_, response::status_code::OK);
}

// We will get the 404 handler due to faulty config
TEST_F(static_request_handler_test, no_root_gets_default)
{
    request r;
    NginxConfig* block_config = new NginxConfig;
    std::shared_ptr<NginxConfigStatement> statement(new NginxConfigStatement);
    block_config->statements_.push_back(statement);

    request_handler* rh = static_request_handler::init("/static", *block_config);

    r.method_ = request::method::GET;
    r.uri_ = "/static/cat.jpg";
    response result;
    result = rh->handle_request(r);
    response_builder b;
    EXPECT_EQ(result.code_, response::status_code::NOT_FOUND);
}

TEST_F(static_request_handler_test, root_ends_with_slash)
{
    request r;
    NginxConfig* block_config = new NginxConfig;
    std::shared_ptr<NginxConfigStatement> statement(new NginxConfigStatement);
    statement->tokens_.push_back("root");
    statement->tokens_.push_back("\"./\"");
    block_config->statements_.push_back(statement);

    request_handler* rh = static_request_handler::init("/static/", *block_config);

    r.method_ = request::method::GET;
    r.uri_ = "/static/catasdf.jpg";
    response result;
    result = rh->handle_request(r);
    response_builder b;
    EXPECT_EQ(result.code_, response::status_code::NOT_FOUND);
}
