#include <iostream>
#include "status_request_handler.h"
#include "config_parser.h"

using namespace boost::filesystem;

status_request_handler::status_request_handler(std::vector<location_parse_result> loc_res)
{
    handlers_info_ = loc_res;
}

request_handler* status_request_handler::init(const std::string& location_path, const NginxConfig& config)
{
    return new status_request_handler(config.get_location_result());
}

response status_request_handler::handle_request(const request& req)
{
    response_builder res;
    if (req.method_ != request::INVALID) {
        res.set_code(response::status_code::OK);
        res.add_header("Content-Type", "text/plain");
        std::string body;

        // list request handlers and URLs
        body += "handlers name \t URL prefixes\r\n";
        for (location_parse_result loc_res : handlers_info_)
            body += loc_res.handler_name + " \t " + loc_res.uri + "\r\n";
        
        body += "\r\n";

        // list requests and response codes
        body += "requests URL \t response codes\r\n";
        std::vector<std::string> records = get_request_records();
        for (std::string record : records)
            body += record + "\r\n";
        res.add_header("Content-Length", std::to_string(body.length()));
        res.add_body(body);
    } else
        res.make_400_error();

    res.make_date_servername_headers();
    return res.get_response();
}

std::vector<std::string> status_request_handler::get_request_records()
{
    std::vector<std::string> records;
    std::vector<std::string> file_names;

    // list all log files in log directory
    path p("../log");
    directory_iterator end_itr;
    for (directory_iterator itr(p); itr != end_itr; ++itr) {
        if (is_regular_file(itr->path())) {
            file_names.push_back(itr->path().string());
        }
    }

    // check each log file
    for (std::string file_name : file_names) {
        std::ifstream file(file_name, std::ios::out);
        // check it is valid file
        if (file.is_open()) {
            std::string line;
            // check each line of log
            while (std::getline(file, line)) {
                // match "for request" in log
                std::size_t found = line.find("for request:");
                if (found!=std::string::npos) {
                    std::string request_url;
                    // get request URL
                    for (std::size_t i=found+13; i<line.size() && line.at(i)!=' '; i++)
                        request_url.push_back(line.at(i));
                    // get response code
                    if (request_url.size() != 0)
                        records.push_back(request_url + "\t\t" + line.substr(found-4, 3));
                } 
            }
            file.close();
        }
    }

    return records;
}
