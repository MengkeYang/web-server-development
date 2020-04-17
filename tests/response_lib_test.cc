#include "gtest/gtest.h"
#include "response.h"
#include <fstream>
#include <string>
#include <streambuf>

class response_test : public ::testing::Test {
    protected:
        response reply;
};

TEST_F(response_test, empty_request_good_parse) {
    std::string empty;
    reply.addResponse(request_parser::result_type::good, empty);
    std::vector<boost::asio::const_buffer> b = reply.getResponse();
    std::string expected = "HTTP/1.1 200 OK";
    std::string response_head(reinterpret_cast<const char*>(boost::asio::buffer_cast<const unsigned char*>(b[0])), expected.length());
    EXPECT_EQ(expected, response_head);
}

TEST_F(response_test, empty_request_bad_parse) {
    std::string empty;
    reply.addResponse(request_parser::result_type::bad, empty);
    std::vector<boost::asio::const_buffer> b = reply.getResponse();
    std::string expected = "HTTP/1.1 400 Bad Request";
    std::string response_head(reinterpret_cast<const char*>(boost::asio::buffer_cast<const unsigned char*>(b[0])), expected.length());
    EXPECT_EQ(expected, response_head);
}

TEST_F(response_test, empty_request_indeterminate_parse) {
    std::string empty;
    reply.addResponse(request_parser::result_type::indeterminate, empty);
    std::vector<boost::asio::const_buffer> b = reply.getResponse();
    std::string expected = "HTTP/1.1 400 Bad Request";
    std::string response_head(reinterpret_cast<const char*>(boost::asio::buffer_cast<const unsigned char*>(b[0])), expected.length());
    EXPECT_EQ(expected, response_head);
}

TEST_F(response_test, request_appended_to_response) {
    std::string request("This is a request, it should be placed verbatim into the response");
    reply.addResponse(request_parser::result_type::good, request);
    std::vector<boost::asio::const_buffer> b = reply.getResponse();
    std::string response_body(reinterpret_cast<const char*>(boost::asio::buffer_cast<const unsigned char*>(b[1])));
    EXPECT_EQ(request, response_body);
}
