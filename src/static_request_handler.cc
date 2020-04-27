#include <iostream>
#include "static_request_handler.h"
#include "config_parser.h"
#include <time.h>

// constructor
StaticRequestHandler::StaticRequestHandler(const NginxConfig &config)
{
    bool root_path_set = false;
    for (const auto &i : config.statements_) {
        if ((i->tokens_)[0] == "server")
            for (const auto &j : (i->child_block_)->statements_)
                if ((j->tokens_)[0] == "root" && (j->tokens_).size() == 2) {
                    root_path_set = true;
                    root = (j->tokens_)[1];
                }
    }
    if (!root_path_set) root = "/";
}

std::string StaticRequestHandler::get_file_name(std::string uri)
{
    int i = uri.rfind("static/");
    std::cout << "This is the file name: " << uri.substr(i + 7) << std::endl;
    return uri.substr(i + 7);

    // int i = uri.length() - 1;
    // while (i >= 0) {
    //     if (uri.compare(i, 1, "/") == 0) break;
    //     i--;
    // }
    // if (i < 0) return uri;
    // return uri.substr(i, std::string::npos);
}

std::string StaticRequestHandler::extension_to_type(
    const std::string &extension)
{
    if (extension == ".zip") return "application/zip";
    if (extension == ".txt") return "text/plain";
    if (extension == ".jpg") return "image/jpeg";
    if (extension == ".png") return "image/png";
    if (extension == ".html" || extension == ".htm") return "text/html";
    if (extension == ".pdf") return "application/pdf";
    if (extension == ".zip") return "application/zip";
    return "application/octet-stream";  // Default for non-text file
}

void StaticRequestHandler::create_response(const request &req,
                                           const std::string &raw_data,
                                           response &result)
{
    if (req.parse_result == request_parser::good) {
        std::cout << "MADE IT HERE\n";
        result.set_status("200 OK");
        // change path
        std::string filename = get_file_name(req.uri);
        std::string uri = root + filename;

        // TODO: not found file
        // check path valid
        std::ofstream fout("/usr/src/projects/WNZA/tests/static_test/log.txt");
        fout << uri << std::endl;
        std::ifstream file(uri, std::ios::binary);
        std::string body;
        if (file.is_open()) {
            char c;
            while (file.get(c)) body += c;
            file.close();
        } else {
            result.set_status("404 Not Found");
            uri = "../tests/404page.html";
            std::ifstream file(uri, std::ios::binary);
            if (file.is_open()) {
                char c;
                while (file.get(c)) body += c;
                file.close();
            }
            std::cerr << "file not found!\n";
        }

        // set MIME
        std::string extension;
        size_t cursor = uri.rfind(".");
        if (cursor != std::string::npos) extension = uri.substr(cursor);
        result.add_data(body);
        result.add_header("Content-Type", extension_to_type(extension));
        result.add_header("Content-Length", std::to_string(body.length()));
    } else
        result.set_status("400 Bad Request");

    char buf[1000];
    memset(buf, 0, sizeof(buf));
    time_t now = time(0);
    struct tm tm = *gmtime(&now);
    strftime(buf, sizeof buf, "%a, %d %b %Y %H:%M:%S %Z", &tm);
    std::string time(buf);
    result.add_header("Date", time);
    result.add_header("Server", "WNZA");
}
