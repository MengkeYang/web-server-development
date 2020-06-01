#include <cstdint>
#include <iostream>
#include <vector>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;

int main(int, char**) {
    // The mongocxx::instance constructor and destructor initialize and shut down the driver,
    // respectively. Therefore, a mongocxx::instance must be created before using the driver and
    // must remain alive for as long as the driver is in use.
    mongocxx::instance inst{};
    mongocxx::client conn{mongocxx::uri{}};

    auto db = conn["wnza"];

    // TODO: fix dates

    // @begin: cpp-insert-a-document
    bsoncxx::document::value website_doc = make_document(
        kvp("google.com",
            make_document(kvp("header", "HTTP/1.1 301 Moved Permanently
                                Location: http://www.google.com/
                                Content-Type: text/html; charset=UTF-8
                                Date: Mon, 01 Jun 2020 09:52:59 GMT
                                Expires: Wed, 01 Jul 2020 09:52:59 GMT
                                Cache-Control: public, max-age=2592000
                                Server: gws
                                Content-Length: 219
                                X-XSS-Protection: 0
                                X-Frame-Options: SAMEORIGIN"),
                          kvp("body", "<HTML><HEAD><meta http-equiv=\"content-type\" content=\"text/html;charset=utf-8\">
                                        <TITLE>301 Moved</TITLE></HEAD><BODY>
                                        <H1>301 Moved</H1>
                                        The document has moved
                                        <A HREF=\"http://www.google.com/\">here</A>.
                                        </BODY></HTML>"),
       
      );
    // We choose to move in our document here, which transfers ownership to insert_one()
    auto res = db["HTTP"].insert_one(std::move(restaurant_doc));
    // @end: cpp-insert-a-document
}