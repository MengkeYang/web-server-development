#include "gtest/gtest.h"
#include <fstream>
#include <iostream>
#include "request.h"
#include "response.h"
#include "config_parser.h"
#include "not_found_request_handler.h"
#include <string.h>
class not_found_request_handler_test : public ::testing::Test
{
protected:
    void SetUp() override
    {
    }
    void TearDown() override { }
    NginxConfigParser config_parser;
    NginxConfig config;
};

TEST_F(not_found_request_handler_test, return_404_code)
{   
    
    config_parser.Parse("test_server_config", &config);
    //request
    request r;
    r.method_ = request::GET;
    r.uri_ = "/nomatch";
   
    not_found_request_handler not_found_handler;
    response result;
    result = not_found_handler.handle_request(r);
    
    EXPECT_EQ(result.code_,response::status_code::NOT_FOUND);

}

