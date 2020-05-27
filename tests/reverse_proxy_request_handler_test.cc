#include "gtest/gtest.h"
#include "request.h"
#include "response.h"
#include "config_parser.h"
#include "reverse_proxy_request_handler.h"
#include <string.h>

class reverse_proxy_request_handler_test : public ::testing::Test
{
protected:
    void SetUp() override
    {   
    }
    void TearDown() override { }
};
	
TEST_F(reverse_proxy_request_handler_test, test_request_handling)
{
    NginxConfig* block_config = new NginxConfig;
    std::shared_ptr<NginxConfigStatement> statement(new NginxConfigStatement); 
    statement->tokens_.push_back("dest");
    statement->tokens_.push_back("\"http://www.ucla.edu/\"");
    block_config->statements_.push_back(statement);

    //request
    request r;
    r.method_ = request::GET;
    r.uri_ = "/proxy";
	
    reverse_proxy_request_handler reverse_proxy_handler(block_config, "/proxy/");
    response result;
    result = reverse_proxy_handler.handle_request(r);
    EXPECT_EQ(result.code_,response::status_code::OK);
    EXPECT_TRUE(result.headers_.size() > 1);
}

TEST_F(reverse_proxy_request_handler_test, test_bad_request)
{   
    NginxConfig* block_config = new NginxConfig;
    std::shared_ptr<NginxConfigStatement> statement(new NginxConfigStatement); 
    statement->tokens_.push_back("dest");
    statement->tokens_.push_back("http://www.this_website_does_not_exist_wkwkwk.wkw/");
    block_config->statements_.push_back(statement);
	
    //request
    request r;
    r.method_ = request::GET;
    r.uri_ = "/proxy";
	
    reverse_proxy_request_handler reverse_proxy_handler(block_config, "/proxy/");
    response result;
    result = reverse_proxy_handler.handle_request(r);
    std::cout << result.code_ << std::endl;
    EXPECT_EQ(result.code_,response::status_code::BAD_REQ);
}
	
	
TEST_F(reverse_proxy_request_handler_test, reverse_proxy_init)
{   
    NginxConfig* block_config = new NginxConfig;
    std::shared_ptr<NginxConfigStatement> statement(new NginxConfigStatement); 
    statement->tokens_.push_back("dest");
    statement->tokens_.push_back("\"https://www.google.com/\"");
    block_config->statements_.push_back(statement);
	
    //request
    request r;
    r.method_ = request::GET;
    r.uri_ = "/proxy";
	
    reverse_proxy_request_handler reverse_proxy_handler(block_config, "/proxy/");
    EXPECT_TRUE(reverse_proxy_handler.init("/", *block_config) != 0);
}
