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

#include "log_helper.h"
#include "server.h"
#include "config_parser.h"

int main(int argc, char* argv[])
{
    log_helper* log = new log_helper();
    try {
        // run bin/server_executable port_number
        if (argc != 2) {
            // std::cerr << "Usage: async_tcp_echo_server <port>\n";
            log->log_error_file(
                "Argument count mismatch. Usage: async_tcp_echo_server "
                "config_file");
            return 1;
        }

        // parse config
        NginxConfigParser config_parser;
        NginxConfig config;
        log->log_trace_file("server config parsing begins");
        if (!config_parser.Parse(argv[1], &config)) {
            // std::cerr << "Parsing config failed!\n";
            log->log_error_file(" config parsing failed");
            return 1;
        }
        // parse port number from config
        int port;
        if ((port = config.parse_port()) == -1) {
            // std::cerr << "The port numbers range from 0 to 65535.\n";
            log->log_error_file("the port numbers range from 0 to 65535");
            return 1;
        }

        log->log_trace_file("server config parsing successed");
        // use boost::asio to control TCP flow
        boost::asio::io_service io_service;

        using namespace std;  // For atoi.
        server s(io_service, (short)port, log);
        log->log_trace_file("server startup successed");

        // run server
        io_service.run();
    } catch (std::exception& e) {
        if (std::strcmp(e.what(), "bind: Permission denied") == 0) {
            // std::cerr << "port forbidden please choose another port.\n";
            log->log_error_file("port forbidden please choose another port");
        } else {
            log->log_error_file(e.what());
            delete log;
            return 1;
        }
    }

    return 0;
}
