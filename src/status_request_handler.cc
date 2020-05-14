#include "status_request_handler.h"


request_handler* status_request_handler::init(const std::string& location_path, const NginxConfig& config)
{
    return new status_request_handler();
}


response status_request_handler::handle_request(const request& req)
{
    response_builder res;
    interval_len = 50;
    if (req.method_ != request::INVALID) {
        res.set_code(response::status_code::OK);
        res.add_header("Content-Type", "text/plain");
        std::stringstream body;
        std::stringstream records = get_request_records();

        // list request handlers and URLs
        body << std::left << std::setfill(' ') 
             << std::setw(interval_len) << "handlers name" << "URL prefixes\r\n";
        body << all_handlers.str();
        
        body << "\r\n";

        // list requests and response codes
        body << std::left << std::setfill(' ') 
             << std::setw(interval_len) << "requests URL" << "response codes\r\n";
        body << records.str();
        res.add_header("Content-Length", std::to_string(body.str().length()));
        res.add_body(body.str());
        body.str("");
        records.str("");
        all_handlers.str("");
    } else
        res.make_400_error();

    res.make_date_servername_headers();
    return res.get_response();
}


std::stringstream status_request_handler::get_request_records()
{
    std::stringstream records;
    records.str("");
    std::vector<std::string> file_names;

    bool handlers_found = false;
    // list all log files in log directory
    boost::filesystem::path p("../log");
    boost::filesystem::directory_iterator end_itr;
    for (boost::filesystem::directory_iterator itr(p); itr != end_itr; ++itr) {
        if (boost::filesystem::is_regular_file(itr->path())) {
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
                // if need all handlers: match "all handlers: "
                if (!handlers_found) {
                    std::size_t found = line.find("all handlers:");
                    if (found!=std::string::npos) {
                        int count = 0;
                        all_handlers << std::left << std::setfill(' ') << std::setw(interval_len);
                        std::string temp;
                        for (std::size_t i=found+14; i<line.size(); i++) {
                            if (line.at(i) == ' ') {
                                if (count%2 == 0) {
                                    all_handlers << temp;
                                }
                                else {
                                    all_handlers << temp << "\r\n";
                                    all_handlers << std::left << std::setfill(' ') << std::setw(interval_len);
                                }
                                temp.clear();
                                count++;
                            } 
                            else
                                temp.push_back(line.at(i));
                        }
                        handlers_found = true;
                    } 
                }

                // match "for request" in log
                std::size_t found = line.find("for request:");
                if (found!=std::string::npos) {
                    std::string request_url;
                    // get request URL
                    for (std::size_t i=found+13; i<line.size() && line.at(i)!=' '; i++)
                        request_url.push_back(line.at(i));
                    // get response code
                    if (request_url.size() != 0)
                        records << std::left << std::setfill(' ') 
                                << std::setw(interval_len) << request_url << line.substr(found-4, 3) << "\r\n";
                } 
            }
            file.close();
        }
    }
    return records;
}
