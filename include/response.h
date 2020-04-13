#ifndef WNZA_RESPONSE_H_
#define WNZA_RESPONSE_H_

#include <vector>
#include <boost/asio.hpp>
#include "request_parser.h"

class response
{
    std::string header;
    std::string body;

public:
    std::vector<boost::asio::const_buffer> getResponse();
    void addResponse(request_parser::result_type r, std::string& b);
};

#endif  // WNZA_RESPONSE_H_
