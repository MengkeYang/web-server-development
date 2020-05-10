#ifndef WNZA_REQUEST_HANDLER_H_
#define WNZA_REQUEST_HANDLER_H_
#include <string>
#include <memory>

class request;
class response;
class NginxConfig;

class request_handler
{
public:
    static std::unique_ptr<request_handler> init(const NginxConfig& config) {};
    virtual void create_response(const request& req, response& result) = 0;
    virtual ~request_handler() {};
};
#endif  // WNZA_REQUEST_HANDLER_H_
