#ifndef WNZA_RESPONSE_H_
#define WNZA_RESPONSE_H_

#include <boost/asio.hpp>
#include <vector>
#include <string>
#include <unordered_map>

struct buffer_response {
    std::vector<boost::asio::const_buffer> bufs;
    std::shared_ptr<std::string> head;
    std::shared_ptr<std::string> body;
};

class response
{
public:
    enum status_code { BAD_REQ, NOT_FOUND, OK };
    buffer_response build_response();
    void set_code(status_code code);
    void add_header(std::string key, std::string val);
    void add_body(std::string data);
    void make_400_error();
    void make_404_error();
    void make_date_servername_headers();

private:
    std::unordered_map<std::string, std::string> headers_;
    std::string body_;
    status_code code_;
};

#endif  // WNZA_RESPONSE_H_
