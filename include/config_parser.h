// An nginx config file parser.
#ifndef WNZA_CONFIG_PARSER_H_
#define WNZA_CONFIG_PARSER_H_

#include <memory>
#include <string>
#include <vector>
#include <map>

class NginxConfig;
class NginxConfigStatement;
class NginxConfigParser;

struct location_parse_result {
    // "/static" 
    std::string uri;
    //StaticHandler 
    std::string handler_name;
    //root directive
    NginxConfig* block_config;

};

// The driver that parses a config file and generates an NginxConfig.
class NginxConfigParser
{
public:
    NginxConfigParser() {}

    // Take a opened config file or file name (respectively) and store the
    // parsed config in the provided NginxConfig out-param.  Returns true
    // iff the input config file is valid.
    bool Parse(std::istream* config_file, NginxConfig* config);
    bool Parse(const char* file_name, NginxConfig* config);

private:
    enum TokenType {
        TOKEN_TYPE_START = 0,
        TOKEN_TYPE_NORMAL = 1,
        TOKEN_TYPE_START_BLOCK = 2,
        TOKEN_TYPE_END_BLOCK = 3,
        TOKEN_TYPE_COMMENT = 4,
        TOKEN_TYPE_STATEMENT_END = 5,
        TOKEN_TYPE_EOF = 6,
        TOKEN_TYPE_ERROR = 7
    };
    const char* TokenTypeAsString(TokenType type);

    enum TokenParserState {
        TOKEN_STATE_INITIAL_WHITESPACE = 0,
        TOKEN_STATE_SINGLE_QUOTE = 1,
        TOKEN_STATE_DOUBLE_QUOTE = 2,
        TOKEN_STATE_TOKEN_TYPE_COMMENT = 3,
        TOKEN_STATE_TOKEN_TYPE_NORMAL = 4
    };

    TokenType ParseToken(std::istream* input, std::string* value);
};

// The parsed representation of the entire config.
class NginxConfig
{
    

public:
    std::string ToString(int depth = 0);
    std::vector<std::shared_ptr<NginxConfigStatement>> statements_;
    std::vector<location_parse_result> get_location_result() const;
    std::string get_value_from_statement(std::string key) const;
    // parse port number
    int parse_port();
};

// The parsed representation of a single config statement.
class NginxConfigStatement
{
public:
    std::string ToString(int depth);
    std::vector<std::string> tokens_;
    std::unique_ptr<NginxConfig> child_block_;
};

#endif  // WNZA_CONFIG_PARSER_H_
