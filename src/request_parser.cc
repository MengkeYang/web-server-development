// This file is revised from boost HTTP example request_parser.cpp
//

#include "request_parser.h"
#include "request.h"
#include <string>
#include <unordered_map>
#include <iostream>

std::tuple<request_parser::result_type, char*, int> request_parser::parse(
    request& req, char* begin, char* end)
{
    while (begin != end) {
        result_type result = consume(req, *begin++);
        if (result == good || result == bad) {
            if (method_str == "GET") req.method_ = request::method::GET;
            else if (method_str == "POST") req.method_ = request::method::POST;
            else if (method_str == "PUT") req.method_ = request::method::PUT;
            else if (method_str == "HEAD") req.method_ = request::method::HEAD;
            else req.method_ = request::method::INVALID;

            if (begin + body_length <= end)
                return std::make_tuple(result, begin, body_length);
            else
                return std::make_tuple(indeterminate, begin, body_length);
        }
    }
    return std::make_tuple(indeterminate, begin, body_length);
}

request_parser::request_parser() : state_(method_start) {}

void request_parser::reset() {
    state_ = method_start;
    header_key.clear();
    header_val.clear();
    method_str.clear();
    body_length = 0;
}

request_parser::result_type request_parser::consume(request& req, char input)
{
    switch (state_) {
        case method_start:
            if (!is_char(input) || is_ctl(input) || is_tspecial(input)) {
                return bad;
            } else {
                state_ = method;
                method_str.push_back(input);
                return indeterminate;
            }
        case method:
            if (input == ' ') {
                state_ = uri;
                return indeterminate;
            } else if (!is_char(input) || is_ctl(input) || is_tspecial(input)) {
                return bad;
            } else {
                method_str.push_back(input);
                return indeterminate;
            }
        case uri:
            if (input == ' ') {
                state_ = http_version_h;
                return indeterminate;
            } else if (is_ctl(input)) {
                return bad;
            } else {
                req.uri_.push_back(input);
                return indeterminate;
            }
        case http_version_h:
            if (input == 'H') {
                state_ = http_version_t_1;
                req.version_.push_back(input);
                return indeterminate;
            } else {
                return bad;
            }
        case http_version_t_1:
            if (input == 'T') {
                state_ = http_version_t_2;
                req.version_.push_back(input);
                return indeterminate;
            } else {
                return bad;
            }
        case http_version_t_2:
            if (input == 'T') {
                state_ = http_version_p;
                req.version_.push_back(input);
                return indeterminate;
            } else {
                return bad;
            }
        case http_version_p:
            if (input == 'P') {
                state_ = http_version_slash;
                req.version_.push_back(input);
                return indeterminate;
            } else {
                return bad;
            }
        case http_version_slash:
            if (input == '/') {
                req.version_.push_back(input);
                state_ = http_version_major_start;
                return indeterminate;
            } else {
                return bad;
            }
        case http_version_major_start:
            if (is_digit(input)) {
                req.version_.push_back(input);
                state_ = http_version_major;
                return indeterminate;
            } else {
                return bad;
            }
        case http_version_major:
            if (input == '.') {
                req.version_.push_back(input);
                state_ = http_version_minor_start;
                return indeterminate;
            } else if (is_digit(input)) {
                req.version_.push_back(input);
                return indeterminate;
            } else {
                return bad;
            }
        case http_version_minor_start:
            if (is_digit(input)) {
                req.version_.push_back(input);
                state_ = http_version_minor;
                return indeterminate;
            } else {
                return bad;
            }
        case http_version_minor:
            if (input == '\r') {
                state_ = expecting_newline_1;
                return indeterminate;
            } else if (is_digit(input)) {
                req.version_.push_back(input);
                return indeterminate;
            } else {
                return bad;
            }
        case expecting_newline_1:
            if (input == '\n') {
                state_ = header_line_start;
                return indeterminate;
            } else {
                return bad;
            }
        case header_line_start:
            if (input == '\r') {
                state_ = expecting_newline_3;
                return indeterminate;
            } else if (!req.headers_.empty() &&
                       (input == ' ' || input == '\t')) {
                state_ = header_lws;
                return indeterminate;
            } else if (!is_char(input) || is_ctl(input) || is_tspecial(input)) {
                return bad;
            } else {
                if (header_key != "") {
                    req.headers_.insert(std::pair<std::string, std::string>(
                        header_key, header_val));
                    if (header_key == "Content-Length")
                        body_length = std::stoi(header_val);
                }
                header_key.clear();
                header_val.clear();
                header_key.push_back(input);
                state_ = header_name;
                return indeterminate;
            }
        case header_lws:
            if (input == '\r') {
                state_ = expecting_newline_2;
                return indeterminate;
            } else if (input == ' ' || input == '\t') {
                return indeterminate;
            } else if (is_ctl(input)) {
                return bad;
            } else {
                state_ = header_value;
                header_val.push_back(input);
                return indeterminate;
            }
        case header_name:
            if (input == ':') {
                state_ = space_before_header_value;
                return indeterminate;
            } else if (!is_char(input) || is_ctl(input) || is_tspecial(input)) {
                return bad;
            } else {
                header_key.push_back(input);
                return indeterminate;
            }
        case space_before_header_value:
            if (input == ' ') {
                state_ = header_value;
                return indeterminate;
            } else {
                return bad;
            }
        case header_value:
            if (input == '\r') {
                state_ = expecting_newline_2;
                return indeterminate;
            } else if (is_ctl(input)) {
                return bad;
            } else {
                header_val.push_back(input);
                return indeterminate;
            }
        case expecting_newline_2:
            if (input == '\n') {
                state_ = header_line_start;
                return indeterminate;
            } else {
                return bad;
            }
        case expecting_newline_3:
            if (header_key != "") {
                req.headers_.insert(std::pair<std::string, std::string>(
                    header_key, header_val));
                if (header_key == "Content-Length")
                    body_length = std::stoi(header_val);
                header_key.clear();
                header_val.clear();
            }
            if (input == '\n') {
                if (body_length == 0)
                    return good;
                state_ = body;
                return indeterminate;
            } else {
                return bad;
            }
        case body:
            req.body_ += input;
            body_length--;
            if (body_length == 0)
                return good;
            return indeterminate;
        default:
            return bad;
    }
}

bool request_parser::is_char(int c) { return c >= 0 && c <= 127; }

bool request_parser::is_ctl(int c) { return (c >= 0 && c <= 31) || (c == 127); }

bool request_parser::is_tspecial(int c)
{
    switch (c) {
        case '(':
        case ')':
        case '<':
        case '>':
        case '@':
        case ',':
        case ';':
        case ':':
        case '\\':
        case '"':
        case '/':
        case '[':
        case ']':
        case '?':
        case '=':
        case '{':
        case '}':
        case ' ':
        case '\t':
            return true;
        default:
            return false;
    }
}

bool request_parser::is_digit(int c) { return c >= '0' && c <= '9'; }
