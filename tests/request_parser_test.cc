#include <fstream>
#include <unordered_map>
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

// test request and header structure
TEST_F(request_parser_test, request_structure) {
  std::unordered_map<std::string, std::string> example_headers{std::make_pair("content", "val")};
  request example_request = {
      .method_=request::method::GET,
	  .uri_="/index.html", 
      .http_version_="HTTP/1.1",
	  .headers_=example_headers, 
	  .body_="body part"
  };
  EXPECT_EQ(example_request.uri_, "/index.html");
  EXPECT_EQ(example_request.body_, "body part");
}

// test whether request_parser can parse empty HTTP request 
TEST_F(request_parser_test, empty_request)
{
  test_file.open("./empty_request", std::fstream::in);
  test_file.read(test_array.data(), test_array.size());
  std::tie(result, std::ignore) = request_parser_.parse(request_, test_array.data(), test_array.data() + test_array.size());
  test_file.close();
  EXPECT_EQ(result, 1);
}

// test whether request_parser can parse bad method HTTP request 
TEST_F(request_parser_test, bad_method_request)
{

  test_file.open("./bad_method_request", std::fstream::in);
  test_file.read(test_array.data(), test_array.size());
  std::tie(result, std::ignore) = request_parser_.parse(request_, test_array.data(), test_array.data() + test_array.size());
  test_file.close();
  EXPECT_EQ(result, 1);
}

// test whether request_parser can parse valid header_line but invalid method
TEST_F(request_parser_test, valid_header_bad_method_request)
{
  test_array = {'W','H','A','T',' ','/',' ','H','T','T','P','/','1','.','1','\r','\n','\r','\n'};
  std::tie(result, std::ignore) = request_parser_.parse(request_, test_array.data(), test_array.data() + test_array.size());
  test_file.close();
  bool test = request_.method_ == request::method::INVALID;
  EXPECT_EQ(test, true);
}

// test whether request_parser can parse invalid first line
TEST_F(request_parser_test, bad_header_line)
{
  test_array = {'W','H','A','T',' ','/',' ','H','T','T','P','/','1','.','1',' ','A','\r','\n','\r','\n'};
  std::tie(result, std::ignore) = request_parser_.parse(request_, test_array.data(), test_array.data() + test_array.size());
  test_file.close();
  EXPECT_EQ(result, 1);
}

// test whether request_parser can parse bad uri HTTP request 
TEST_F(request_parser_test, bad_uri_request)
{
  test_array = {'G','E','T',' ',25};
  std::tie(result, std::ignore) = request_parser_.parse(request_, test_array.data(), test_array.data() + test_array.size());
  test_file.close();
  EXPECT_EQ(result, 1);
}

// test whether request_parser can parse HTTP letters:H error 
TEST_F(request_parser_test, bad_H_request)
{
  test_array = {'G','E','T',' ','/',' ','A'};
  std::tie(result, std::ignore) = request_parser_.parse(request_, test_array.data(), test_array.data() + test_array.size());
  test_file.close();
  EXPECT_EQ(result, 1);
}

// test whether request_parser can parse HTTP letters:T1 error 
TEST_F(request_parser_test, bad_T1_request)
{
  test_array = {'G','E','T',' ','/',' ','H','H'};
  std::tie(result, std::ignore) = request_parser_.parse(request_, test_array.data(), test_array.data() + test_array.size());
  test_file.close();
  EXPECT_EQ(result, 1);
}

// test whether request_parser can parse HTTP letters:T2 error 
TEST_F(request_parser_test, bad_T2_request)
{
  test_array = {'G','E','T',' ','/',' ','H','T','H'};
  std::tie(result, std::ignore) = request_parser_.parse(request_, test_array.data(), test_array.data() + test_array.size());
  test_file.close();
  EXPECT_EQ(result, 1);
}

// test whether request_parser can parse HTTP letters:P error 
TEST_F(request_parser_test, bad_P_request)
{
  test_array = {'G','E','T',' ','/',' ','H','T','T','T'};
  std::tie(result, std::ignore) = request_parser_.parse(request_, test_array.data(), test_array.data() + test_array.size());
  test_file.close();
  EXPECT_EQ(result, 1);
}

// test whether request_parser can parse HTTP letters:slash error 
TEST_F(request_parser_test, bad_slash_request)
{
  test_array = {'G','E','T',' ','/',' ','H','T','T','P','P'};
  std::tie(result, std::ignore) = request_parser_.parse(request_, test_array.data(), test_array.data() + test_array.size());
  test_file.close();
  EXPECT_EQ(result, 1);
}

// test whether request_parser can parse HTTP letters:number after slash error 
TEST_F(request_parser_test, bad_major_request)
{
  test_array = {'G','E','T',' ','/',' ','H','T','T','P','/','/'};
  std::tie(result, std::ignore) = request_parser_.parse(request_, test_array.data(), test_array.data() + test_array.size());
  test_file.close();
  EXPECT_EQ(result, 1);
}

// test whether request_parser can parse HTTP letters:without dot error 
TEST_F(request_parser_test, bad_major_continue_request)
{
  test_array = {'G','E','T',' ','/',' ','H','T','T','P','/','1','2','/'};
  std::tie(result, std::ignore) = request_parser_.parse(request_, test_array.data(), test_array.data() + test_array.size());
  test_file.close();
  EXPECT_EQ(result, 1);
}

// test whether request_parser can parse HTTP letters:without minor version error 
TEST_F(request_parser_test, bad_minor_request)
{
  test_array = {'G','E','T',' ','/',' ','H','T','T','P','/','1','.','/'};
  std::tie(result, std::ignore) = request_parser_.parse(request_, test_array.data(), test_array.data() + test_array.size());
  test_file.close();
  EXPECT_EQ(result, 1);
}

// test whether request_parser can parse HTTP letters:\r after minor version error 
TEST_F(request_parser_test, bad_minor_continue_request)
{
  test_array = {'G','E','T',' ','/',' ','H','T','T','P','/','1','.','1','1','/'};
  std::tie(result, std::ignore) = request_parser_.parse(request_, test_array.data(), test_array.data() + test_array.size());
  test_file.close();
  EXPECT_EQ(result, 1);
}

// test whether request_parser can parse HTTP request new line errors 
TEST_F(request_parser_test, new_line1_request)
{
  test_file.open("./new_line1_request", std::fstream::in);
  test_file.read(test_array.data(), test_array.size());
  std::tie(result, std::ignore) = request_parser_.parse(request_, test_array.data(), test_array.data() + test_array.size());
  test_file.close();
  EXPECT_EQ(result, 1);
}

// test whether request_parser can parse HTTP request bad header errors 
TEST_F(request_parser_test, bad_header_request)
{
  test_array = {'G','E','T',' ','/',' ','H','T','T','P','/','1','.','1','\r','\n',25};
  std::tie(result, std::ignore) = request_parser_.parse(request_, test_array.data(), test_array.data() + test_array.size());
  test_file.close();
  EXPECT_EQ(result, 1);
}

// test whether request_parser can parse HTTP request header_lws and end parsing 
TEST_F(request_parser_test, header_lws_request)
{
  test_array = {'G','E','T',' ','/',' ','H','T','T','P','/','1','.','1','\r','\n',' ',' ','\r'};
  std::tie(result, std::ignore) = request_parser_.parse(request_, test_array.data(), test_array.data() + test_array.size());
  test_file.close();
  EXPECT_EQ(result, 1);
}

// test whether request_parser can parse HTTP request header_lws and transit to header value 
TEST_F(request_parser_test, header_lws_ctl_request)
{
  test_array = {'G','E','T',' ','/',' ','H','T','T','P','/','1','.','1','\r','\n',' '};
  std::tie(result, std::ignore) = request_parser_.parse(request_, test_array.data(), test_array.data() + test_array.size());
  test_file.close();
  EXPECT_EQ(result, 1);
}

// test whether request_parser can parse HTTP request header_lws errors 
TEST_F(request_parser_test, header_lws_other_request)
{
  test_array = {'G','E','T',' ','/',' ','H','T','T','P','/','1','.','1','\r','\n',' ','b'};
  std::tie(result, std::ignore) = request_parser_.parse(request_, test_array.data(), test_array.data() + test_array.size());
  test_file.close();
  EXPECT_EQ(result, 1);
}

// test whether request_parser can parse HTTP request header name errors 
TEST_F(request_parser_test, header_name_request)
{
  test_array = {'G','E','T',' ','/',' ','H','T','T','P','/','1','.','1','\r','\n','d',25};
  std::tie(result, std::ignore) = request_parser_.parse(request_, test_array.data(), test_array.data() + test_array.size());
  test_file.close();
  EXPECT_EQ(result, 1);
}

// test whether request_parser can parse HTTP request space before header value errors 
TEST_F(request_parser_test, space_before_request)
{
  test_array = {'G','E','T',' ','/',' ','H','T','T','P','/','1','.','1','\r','\n','d',':',':'};
  std::tie(result, std::ignore) = request_parser_.parse(request_, test_array.data(), test_array.data() + test_array.size());
  test_file.close();
  EXPECT_EQ(result, 1);
}

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
// TODO: Add support for this
//TEST_F(request_parser_test, different_method_request)
//{
//  test_file.open("./different_method_request", std::fstream::in);
//  test_file.read(test_array.data(), test_array.size());
//  std::tie(result, std::ignore) = request_parser_.parse(request_, test_array.data(), test_array.data() + test_array.size());
//  test_file.close();
//  EXPECT_EQ(result, 0);
//}
