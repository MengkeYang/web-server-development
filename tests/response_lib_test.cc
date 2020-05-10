#include "gtest/gtest.h"
#include "response.h"
#include "request.h"
#include <fstream>
#include <string>
#include <streambuf>

class response_test : public ::testing::Test {
    protected:
        response reply;
};

TEST_F(response_test, empty_request_good_parse) {
    reply.set_code(response::status_code::OK);
    buffer_response b = reply.build_response();
    std::string expected = "HTTP/1.1 200 OK";
    std::string response_head(reinterpret_cast<const char*>(boost::asio::buffer_cast<const unsigned char*>(b.bufs[0])), expected.length());
    EXPECT_EQ(expected, response_head);
}

TEST_F(response_test, empty_request_bad_parse) {
    std::string bad("400 Bad Request");
    reply.set_code(response::status_code::BAD_REQ);
    buffer_response b = reply.build_response();
    std::string expected = "HTTP/1.1 400 Bad Request";
    std::string response_head(reinterpret_cast<const char*>(boost::asio::buffer_cast<const unsigned char*>(b.bufs[0])), expected.length());
    EXPECT_EQ(expected, response_head);
}

TEST_F(response_test, request_appended_to_response) {
    std::string request("This is a request, it should be placed verbatim into the response");
    reply.add_body(request);
    reply.set_code(response::status_code::OK);
    buffer_response b = reply.build_response();
    std::string response_body(reinterpret_cast<const char*>(boost::asio::buffer_cast<const unsigned char*>(b.bufs[1])));
    EXPECT_EQ(request, response_body);
}
