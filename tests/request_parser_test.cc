#include <fstream>
#include "gtest/gtest.h"
#include "request.h"
#include "request_parser.h"

class request_parser_test : public ::testing::Test
{
protected:
    void SetUp() override {
    }
    request request_;
    request_parser request_parser_;
    request_parser::result_type result;
    std::fstream test_file;
    std::array<char, 8192> test_array;
};

// test whether request_parser can parse HTTP request without body 
TEST_F(request_parser_test, empty_body_request)
{
  test_file.open("./empty_body_request", std::fstream::in);
  test_file.read(test_array.data(), test_array.size());
  std::tie(result, std::ignore) = request_parser_.parse(request_, test_array.data(), test_array.data() + test_array.size());
  test_file.close();
  EXPECT_EQ(result, 0);
}

// test whether request_parser can parse HTTP request with headers 
TEST_F(request_parser_test, with_header_request)
{
  test_file.open("./with_header_request", std::fstream::in);
  test_file.read(test_array.data(), test_array.size());
  std::tie(result, std::ignore) = request_parser_.parse(request_, test_array.data(), test_array.data() + test_array.size());
  test_file.close();
  EXPECT_EQ(result, 0);
}

// test whether request_parser can parse HTTP request with different method and uri 
TEST_F(request_parser_test, different_method_request)
{
  test_file.open("./different_method_request", std::fstream::in);
  test_file.read(test_array.data(), test_array.size());
  std::tie(result, std::ignore) = request_parser_.parse(request_, test_array.data(), test_array.data() + test_array.size());
  test_file.close();
  EXPECT_EQ(result, 0);
}
