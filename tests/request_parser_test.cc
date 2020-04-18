#include <fstream>
#include <vector>
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
  header example_header = {.name="content", .value="content value"};
  std::vector<header> example_headers{example_header};
  request example_request = {
	  .method="GET", 
	  .uri="/index.html", 
	  .http_version_major=1, 
	  .http_version_minor=1, 
	  .headers=example_headers, 
	  .body="body part"
  };
  EXPECT_EQ(example_header.name, "content");
  EXPECT_EQ(example_header.value, "content value");
  EXPECT_EQ(example_request.method, "GET");
  EXPECT_EQ(example_request.uri, "/index.html");
  EXPECT_EQ(example_request.http_version_major, 1);
  EXPECT_EQ(example_request.http_version_minor, 1);
  EXPECT_EQ(example_request.body, "body part");
}

/*
// unit test for request_parser::is_tspecial
TEST_F(request_parser_test, parser_tspecial)
{
  bool result = request_parser_.is_tspecial(38);
  EXPECT_EQ(result, false);
  result = request_parser_.is_tspecial('(');
  EXPECT_EQ(result, true);
}

// unit test for request_parser::is_char
TEST_F(request_parser_test, parser_char)
{
  bool result = request_parser_.is_char('G');
  EXPECT_EQ(result, true);
}

// unit test for request_parser::is_ctl
TEST_F(request_parser_test, parser_ctl)
{
  bool result = request_parser_.is_ctl('G');
  EXPECT_EQ(result, false);
}


// unit test for request_parser::consume 
TEST_F(request_parser_test, parser_consume)
{
  request_parser::result_type consume_result = request_parser_.consume(request_, 'G');
  EXPECT_EQ(consume_result, 2);
}
*/

// unit test for request_parser::is_digit
TEST_F(request_parser_test, parser_digit)
{
  bool result = request_parser::is_digit('G');
  EXPECT_EQ(result, false);
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
  request_.headers.push_back(header());
  std::tie(result, std::ignore) = request_parser_.parse(request_, test_array.data(), test_array.data() + test_array.size());
  test_file.close();
  EXPECT_EQ(result, 1);
}

// test whether request_parser can parse HTTP request header_lws and transit to header value 
TEST_F(request_parser_test, header_lws_ctl_request)
{
  test_array = {'G','E','T',' ','/',' ','H','T','T','P','/','1','.','1','\r','\n',' '};
  request_.headers.push_back(header());
  std::tie(result, std::ignore) = request_parser_.parse(request_, test_array.data(), test_array.data() + test_array.size());
  test_file.close();
  EXPECT_EQ(result, 1);
}

// test whether request_parser can parse HTTP request header_lws errors 
TEST_F(request_parser_test, header_lws_other_request)
{
  test_array = {'G','E','T',' ','/',' ','H','T','T','P','/','1','.','1','\r','\n',' ','b'};
  request_.headers.push_back(header());
  std::tie(result, std::ignore) = request_parser_.parse(request_, test_array.data(), test_array.data() + test_array.size());
  test_file.close();
  EXPECT_EQ(result, 1);
}

// test whether request_parser can parse HTTP request header name errors 
TEST_F(request_parser_test, header_name_request)
{
  test_array = {'G','E','T',' ','/',' ','H','T','T','P','/','1','.','1','\r','\n','d',25};
  request_.headers.push_back(header());
  std::tie(result, std::ignore) = request_parser_.parse(request_, test_array.data(), test_array.data() + test_array.size());
  test_file.close();
  EXPECT_EQ(result, 1);
}

// test whether request_parser can parse HTTP request space before header value errors 
TEST_F(request_parser_test, space_before_request)
{
  test_array = {'G','E','T',' ','/',' ','H','T','T','P','/','1','.','1','\r','\n','d',':',':'};
  request_.headers.push_back(header());
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
TEST_F(request_parser_test, different_method_request)
{
  test_file.open("./different_method_request", std::fstream::in);
  test_file.read(test_array.data(), test_array.size());
  std::tie(result, std::ignore) = request_parser_.parse(request_, test_array.data(), test_array.data() + test_array.size());
  test_file.close();
  EXPECT_EQ(result, 0);
}
