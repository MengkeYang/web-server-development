#include "echo_request_handler.h"
#include "request.h"
#include "response.h"
#include <time.h>

void echo_request_handler::create_response(const request& req,
                                           const std::string& raw_data,
                                           response& result)
{
    if (req.parse_result == request_parser::good) {
        result.set_status("200 OK");
        result.add_header("Content-Length", std::to_string(raw_data.length()));
        result.add_header("Content-Type", "text/plain");
        result.add_data(raw_data);
    } else
        result.make_400_error();

    char buf[1000];
    time_t now = time(0);
    struct tm tm = *gmtime(&now);
    strftime(buf, sizeof buf, "%a, %d %b %Y %H:%M:%S %Z", &tm);
    std::string time(buf);
    result.add_header("Date", time);
    result.add_header("Server", "WNZA");
}
