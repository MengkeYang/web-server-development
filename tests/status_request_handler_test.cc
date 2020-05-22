#include "gtest/gtest.h"
#include "request.h"
#include "response.h"
#include "config_parser.h"
#include "status_request_handler.h"
#include <string.h>


class status_request_handler_test : public ::testing::Test
{
protected:
    void SetUp() override
    {
        config_parser.Parse("test_server_config", &config);
        r.method_ = request::GET;
        r.uri_ = "/status";
        
    }
    void TearDown() override { }
    NginxConfigParser config_parser;
    NginxConfig config;
    request r;
    status_request_handler status_handler;
};


TEST_F(status_request_handler_test, list_request_records)
{   
    std::stringstream records = status_handler.get_request_records();
    // TODO: actually read log file and compare
    EXPECT_TRUE(records.str().size()!=0);
}


TEST_F(status_request_handler_test, list_handlers)
{   
    
    response result = status_handler.handle_request(r);
    std::string body = result.body_;
    EXPECT_TRUE(body.find("StaticHandler") != std::string::npos);
    EXPECT_TRUE(body.find("EchoHandler") != std::string::npos);
    EXPECT_TRUE(body.find("StatusHandler") != std::string::npos);
    EXPECT_TRUE(body.find("NotFoundHandler") != std::string::npos);
    EXPECT_TRUE(body.find("ProxyHandler") != std::string::npos);
}