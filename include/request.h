// This is revised from boost HTTP example request.hpp
#ifndef WNZA_REQUEST_H_
#define WNZA_REQUEST_H_

#include <string>
#include <vector>


// One header line
struct header
{
  std::string name;
  std::string value;
};


// A request received from a client.
struct request
{
  std::string method;
  std::string uri;
  int http_version_major;
  int http_version_minor;
  std::vector<header> headers;
  std::string body;
};
#endif  // WNZA_REQUEST_H_