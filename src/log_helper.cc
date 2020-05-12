#include "log_helper.h"
#include "request.h"
#include "response.h"

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;
using namespace logging::trivial;

log_helper::log_helper() { init(); }

void log_helper::init()
{
    // Log output lines include a timestamp, the current thread ID, a severity
    // level, the process ID, the log line num and a message.
    static const std::string COMMON_FMT(
        "%TimeStamp% [%ThreadID%] [%Severity%] "
        "[%ProcessID%] [%LineID%] %Message%");

    logging::register_simple_formatter_factory<logging::trivial::severity_level,
                                               char>("Severity");

    // Output message to console
    logging::add_console_log(std::cout, logging::keywords::format = COMMON_FMT,
                             logging::keywords::auto_flush = true);

    // Output message to file
    logging::add_file_log(keywords::file_name = "../log/sample_log_%N.log",
                          // log file rotation: rotates when a day’s log file
                          // size reached 10mb or at midnight every day.
                          keywords::rotation_size = 10 * 1024 * 1024,
                          keywords::time_based_rotation =
                              sinks::file::rotation_at_time_point(0, 0, 0),
                          keywords::auto_flush = true,
                          keywords::open_mode = std::ios::app,
                          keywords::format = COMMON_FMT);

    logging::add_common_attributes();

    logging::core::get()->set_filter(logging::trivial::severity >=
                                     logging::trivial::trace);
}

void log_helper::log_trace_file(std::string trace_message)
{
    BOOST_LOG_SEV(lg, trace) << "Trace: " << trace_message;
}
void log_helper::log_error_file(std::string error_message)
{
    BOOST_LOG_SEV(lg, error) << "Error: " << error_message;
}
void log_helper::log_warning_file(std::string warning_message)
{
    BOOST_LOG_SEV(lg, warning) << "Warning: " << warning_message;
}

void log_helper::log_request_info(request req, tcp::socket* socket)
{
    std::string methods[] = {"GET", "POST", "PUT", "INVALID"};
    std::stringstream str_stream;
    str_stream << "Trace: ";
    str_stream << methods[req.method_] << " " << req.uri_ << " HTTP ";
    for (auto&& h : req.headers_) {
        str_stream << " " << h.first << ":" << h.second;
    }
    str_stream << " IP: " << socket->remote_endpoint().address().to_string();
    str_stream << " Port: " << socket->remote_endpoint().port();
    BOOST_LOG_SEV(lg, trace) << str_stream.str();
}

void log_helper::log_response_info(request req, response res, tcp::socket* socket)
{
    std::string codes[] = {"400", "404", "200"};
    std::stringstream str_stream;
    str_stream << "Trace: ";
    str_stream << codes[res.code_] << " for request: " << req.uri_;
    BOOST_LOG_SEV(lg, trace) << str_stream.str();
}
