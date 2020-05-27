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
    static request_handler* init(const std::string& location_path, const NginxConfig& config) {};
    virtual response handle_request(const request& req) = 0;
    virtual std::string get_handler_name(){};
    virtual ~request_handler() {};
};
#endif  // WNZA_REQUEST_HANDLER_H_
