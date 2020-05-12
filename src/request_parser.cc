// This file is revised from boost HTTP example request_parser.cpp
//

#include "request_parser.h"
#include "request.h"
#include <string>

std::tuple<request_parser::result_type, char*> request_parser::parse(
    request& req, char* begin, char* end)
{
    std::vector<std::vector<std::string>> lines;
    std::string tok;
    std::vector<std::string> line;
    bool valid = false;
    bool reading_rhs = false;
    static const std::unordered_map<std::string, request::method>
        str_to_method = {{"GET", request::method::GET},
                         {"HEAD", request::method::HEAD},
                         {"POST", request::method::POST},
                         {"PUT", request::method::PUT}};
    while (begin != end) {
        if (reading_rhs) {
            if (*begin == '\r' || *begin == '\n') {
                if (tok == "") {
                    reading_rhs = false;
                } else {
                    line.push_back(tok);
                    lines.push_back(line);
                    line.clear();
                    tok = "";
                }
            } else {
                tok += *begin;
            }
            begin++;
            continue;
        }

        if (*begin == ':') {
            reading_rhs = true;
            line.push_back(tok);
            tok = "";
            begin++;
            continue;
        }
        if (*begin != ' ' && *begin != '\r' && *begin != '\n') tok += *begin;
        if (*begin == ' ' && tok.length() > 0) {
            line.push_back(tok);
            tok = "";
        }
        if (*begin == '\n') {
            if (tok == "") {
                begin++;
                valid = true;
                break;  // We have reached '\r\n\r\n'
            }
            line.push_back(tok);
            tok = "";
            lines.push_back(line);
            line.clear();
        }
        begin++;
    }

    if (valid && lines.size() > 0) {
        if (lines[0].size() != 3) {
            req.method_ = request::method::INVALID;
            return std::make_tuple(bad, begin);
        }
        std::unordered_map<std::string, request::method>::const_iterator it =
            str_to_method.find(lines[0][0]);
        if (it == str_to_method.end())
            req.method_ = request::method::INVALID;
        else
            req.method_ = it->second;

        req.uri_ = lines[0][1];
        req.http_version_ = lines[0][2];

        for (int i = 1; i < lines.size(); i++) {
            // lines[i][0].pop_back();  // Removing the ':'
            if (lines[i][1].at(0) == ' ') lines[i][1].erase(0, 1);
            req.headers_.insert(std::make_pair(lines[i][0], lines[i][1]));
        }
        req.body_ = std::string(begin, end);
        return std::make_tuple(good, begin);
    } else
        req.method_ = request::method::INVALID;
    return std::make_tuple(bad, begin);
}
