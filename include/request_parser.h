// This file is revised from boost HTTP example request_parser.hpp
#ifndef WNZA_REQUEST_PARSER_H_
#define WNZA_REQUEST_PARSER_H_

#include <tuple>

struct request;

class request_parser {
public:
  // Result of parse.
  enum result_type { good, bad, indeterminate };

  // Parse some data. The enum return value is good when a complete request has
  // been parsed, bad if the data is invalid, indeterminate when more data is
  // required. The InputIterator return value indicates how much of the input
  // has been consumed.
  std::tuple<result_type, char*> parse(request& req, char* begin, char* end);

};	
#endif  // WNZA_REQUEST_PARSER_H_
