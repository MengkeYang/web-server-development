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
    std::vector<location_parse_result> loc_res;
};
/*
// test common_api_config
TEST_F(config_parser_test, common_api_config)
{
  if(!config_parser.Parse("./common_api_config", &config))
  {
    std::cerr << "Parsing config failed!\n";
    return;
  }
  loc_res = config.get_location_result();
  EXPECT_TRUE(loc_res.at(0).handler_name.compare("StaticHandler") == 0);
}
*/

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


TEST_F(config_parser_test, EndSingleQuote) {
  bool success = config_parser.Parse("singlequote_config", &config);
  EXPECT_FALSE(success);
}


TEST_F(config_parser_test, EndDoubleQuote) {
  bool success = config_parser.Parse("doublequote_config", &config);
  EXPECT_FALSE(success);
}

// TEST_F(config_parser_test, MoreBlocks) {
//   bool success = config_parser.Parse("/more_blocks_config", &config);
//   EXPECT_TRUE(success);
// }

// TEST_F(config_parser_test, HttpConfig) {
//   bool success = config_parser.Parse("/requestHttp_config", &config);
//   EXPECT_TRUE(success);
// }

TEST_F(config_parser_test, NoClosingBracket) {
  bool success = config_parser.Parse("./noclose_config", &config);
  EXPECT_FALSE(success);
}


TEST_F(config_parser_test, NestLoopConfig) {
  bool success = config_parser.Parse("nested_loop_config", &config);
  EXPECT_TRUE(success);
}

TEST_F(config_parser_test, InvalidLeftParenthesisConfig) {
  bool success = config_parser.Parse("invalid_left_parenthesis_config", &config);
  EXPECT_FALSE(success);
}

TEST_F(config_parser_test, InvalidRightParenthesisConfig) {
    NginxConfigParser config_parser;
  bool success = config_parser.Parse("invalid_right_parenthesis_config", &config);
  EXPECT_FALSE(success);
}

TEST_F(config_parser_test, NginxConfigToStringNested) {
  bool success = config_parser.Parse("nested_loop_config", &config);
  std::string res =  "foo \"bar\";\nserver {\n  location / {\n    proxy_pass http://localhost:8080;\n  }\n}\n";
  std::string config_string = config.ToString(0);
  bool same = config_string.compare(res)==0;

  EXPECT_TRUE(same);
}

// TEST_F(config_parser_test, NestedPort) {
//   bool success = config_parser.Parse("nested_port", &config);

//   int port = config.parse_port();
//   bool same = port==8080;

//   EXPECT_TRUE(same);
// }

TEST_F(config_parser_test, GetPortNumberNull) {
  bool success = config_parser.Parse("empty_config", &config);

  int port = config.parse_port();
  bool same = port==-1;

  EXPECT_TRUE(same);
}

TEST_F(config_parser_test, EndBlockError) {
  bool success = config_parser.Parse("end_block_error", &config);
  EXPECT_FALSE(success);
}

TEST_F(config_parser_test, ConstructorTest) {
  NginxConfigParser *p = new NginxConfigParser();
  bool success = (p != NULL);
  EXPECT_TRUE(success);
}

TEST_F(config_parser_test, GetPortNumberNoPort) {
  bool success = config_parser.Parse("no_port_config", &config);

  int port = config.parse_port();
  bool same = port==-1;

  EXPECT_TRUE(same);
}
