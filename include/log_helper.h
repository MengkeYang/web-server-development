#ifndef WNZA_LOG_HELPER_H_
#define WNZA_LOG_HELPER_H_

#include <boost/asio.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <mutex>

class request;
class response;
class NginxConfig;
class response_builder;
class connection;

namespace logging = boost::log;
namespace src = boost::log::sources;
using namespace logging::trivial;
using boost::asio::ip::tcp;

class log_helper
{
    std::mutex lock;
public:
    src::severity_logger<severity_level> lg;
    log_helper();

    // Using the standard C++11 singleton implementation
    static log_helper& instance();
    log_helper(const log_helper&) = delete;
    log_helper(log_helper&&) = delete;
    log_helper& operator=(const log_helper&) = delete;
    log_helper& operator=(log_helper&&) = delete;

    void log_trace_file(std::string trace_message);
    void log_error_file(std::string error_message);
    void log_warning_file(std::string warning_message);
    void log_request_info(request req, connection* conn);
    void log_response_info(request req, response res);
    void log_all_handlers(const NginxConfig& config);
    void log_metrics(request req, response res, connection* conn, std::string handler_name);
    void log_response_sent();
    ~log_helper() {}
};
#endif
