
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
#include "request.h"
#include "session.h"

namespace logging = boost::log;
namespace src = boost::log::sources;
using namespace logging::trivial;

class log_helper
{
public:
    src::severity_logger<severity_level> lg;
    log_helper();
    void init();

    void log_trace_file(std::string trace_message);
    void log_error_file(std::string error_message);
    void log_warning_file(std::string warning_message);
    void log_request_info(request req, tcp::socket* socket);
};
#endif