
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

class request;
class response;

namespace logging = boost::log;
namespace src = boost::log::sources;
using namespace logging::trivial;
using boost::asio::ip::tcp;

class log_helper
{
public:
    src::severity_logger<severity_level> lg;
    log_helper();
    virtual void init();

    virtual void log_trace_file(std::string trace_message);
    virtual void log_error_file(std::string error_message);
    virtual void log_warning_file(std::string warning_message);
    virtual void log_request_info(request req, tcp::socket* socket);
    virtual void log_response_info(request req, response res, tcp::socket* socket);
    virtual ~log_helper() {}
};
#endif
