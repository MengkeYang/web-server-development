// This file is revised from boost HTTP example request_parser.hpp
#ifndef WNZA_REQUEST_PARSER_H_
#define WNZA_REQUEST_PARSER_H_

#include <tuple>

struct request;

class request_parser {
public:
  // Construct ready to parse the request method.
  request_parser();

  // Reset to initial parser state.
  void reset();

  static bool is_tspecial(int c);

  static bool is_digit(int c);

  static bool is_char(int c);

  static bool is_ctl(int c);
  
  // Result of parse.
  enum result_type { good, bad, indeterminate };

  // Parse some data. The enum return value is good when a complete request has
  // been parsed, bad if the data is invalid, indeterminate when more data is
  // required. The InputIterator return value indicates how much of the input
  // has been consumed.
  std::tuple<result_type, char*> parse(request& req, char* begin, char* end);

  result_type consume(request& req, char input);

  enum state
  {
    method_start,
    method,
    uri,
    http_version_h,
    http_version_t_1,
    http_version_t_2,
    http_version_p,
    http_version_slash,
    http_version_major_start,
    http_version_major,
    http_version_minor_start,
    http_version_minor,
    expecting_newline_1,
    header_line_start,
    header_lws,
    header_name,
    space_before_header_value,
    header_value,
    expecting_newline_2,
    expecting_newline_3,
  } state_;
};	
#endif  // WNZA_REQUEST_PARSER_H_
