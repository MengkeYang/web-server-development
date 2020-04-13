#include <fstream>
#include "gtest/gtest.h"
#include "config_parser.h"

class config_parser_test : public ::testing::Test
{
protected:
    void SetUp() override {
    }
    NginxConfigParser config_parser;
    NginxConfig config;
    int port;
};

// test whether config_parser can parse HTTP request port 
TEST_F(config_parser_test, example_config)
{
  if(!config_parser.Parse("./example_config", &config))
  {
    std::cerr << "Parsing config failed!\n";
    return;
  }
  port = config.parse_port();
  EXPECT_EQ(port, 8080);
}

// test whether config_parser can parse multi port 
TEST_F(config_parser_test, multiple_ports_config)
{
  if(!config_parser.Parse("./multiple_ports_config", &config))
  {
    std::cerr << "Parsing config failed!\n";
    return;
  }
  port = config.parse_port();
  EXPECT_EQ(port, 8080);
}

// test whether config_parser can parse nested port config 
TEST_F(config_parser_test, nested_config)
{
  if(!config_parser.Parse("./nested_config", &config))
  {
    std::cerr << "Parsing config failed!\n";
    return;
  }
  port = config.parse_port();
  EXPECT_EQ(port, 8080);
}

// test whether config_parser can parse multiple ports nested config 
TEST_F(config_parser_test, multiple_ports_nested_config)
{
  if(!config_parser.Parse("./multiple_ports_nested_config", &config))
  {
    std::cerr << "Parsing config failed!\n";
    return;
  }
  port = config.parse_port();
  EXPECT_EQ(port, 8080);
}
