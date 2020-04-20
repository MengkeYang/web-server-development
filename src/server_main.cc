// 
// This is the main executable of http server,
// which is refactored by boost echo server example
//
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "server.h"

using boost::asio::ip::tcp;

int main(int argc, char* argv[])
{

  try
  {
    // run bin/server_executable port_number
    if (argc != 2)
    {
      std::cerr << "Usage: async_tcp_echo_server <port>\n";
      return 1;
    }
      
    //parse config
    NginxConfigParser config_parser;
    NginxConfig config;
    if(!config_parser.Parse(argv[1], &config))
    {
      std::cerr << "Parsing config failed!\n";
      return 1;
    }
    //parse port number from config
    int port;
    if ((port = config.parse_port()) == -1)
    {
      std::cerr << "The port numbers range from 0 to 65535.\n";
      return 1;
    }

    // use boost::asio to control TCP flow
    boost::asio::io_service io_service;

    using namespace std; // For atoi.
    server s(io_service, (short)port, config);
    //server s(io_service, atoi(argv[1]));

    // run server
    io_service.run();
  }
  catch (std::exception& e)
  {
    if(std::strcmp(e.what(), "bind: Permission denied") == 0)
    {
      std::cerr << "port forbidden please choose another port.\n";
    }
    else
    {
      std::cerr << "Exception: " << e.what() << "\n";
    }
  }

  return 0;
}
