#ifndef WNZA_RESPONSE_H_
#define WNZA_RESPONSE_H_

#include <boost/asio.hpp>
#include <vector>
#include <string>
#include <unordered_map>

class response
{
public:
    enum status_code { BAD_REQ, NOT_FOUND, OK };

    std::unordered_map<std::string, std::string> headers_;
    std::string body_;
    status_code code_;
};

class response_builder
{
private:
    response response_;
    std::vector<boost::asio::const_buffer> bufs_;
    std::shared_ptr<std::string> head_;
    std::shared_ptr<std::string> body_;

public:
    void set_response(response& res);
    response get_response();
    std::vector<boost::asio::const_buffer> build();
    void set_code(response::status_code code);
    void add_header(std::string key, std::string val);
    void add_body(std::string data);
    void make_400_error();
    void make_404_error();
    void make_date_servername_headers();
};

#endif  // WNZA_RESPONSE_H_
