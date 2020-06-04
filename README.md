# WNZA HTTP Server
Weijia, Mengke, Zongze, Adam

[TOC]

## Source Layout
Our directory structure separates header files from their corresponding
implementation files. All implementation files (.cc) are placed into the src/
directory and all header files (.h) are placed into the include/ directory. All
tests and files necessary for the tests are located in the tests/ directory. We
also have dockerfiles placed in the docker/ directory. Finally, output log files
are stored in the log/ directory.


Our source code is organized by the classes which are implemented and then the
main code is placed into `server_main.cc`.
`server_main.cc`:

* Handles all command line arguments, starting a server instance, and
catching any runtime exceptions.

`server.cc`:

* Implementation of the server class which deals with the parsing of the
config file, listening on a specified port, and creating new session
objects for every connection established to the server.

`session.cc`:

* A session holds a connection object which represents the TCP connection
between client and server. A session deals with processing and dispatching
requests to certain request handlers and sending responses by using
methods of the connection class.
* When data is being written to a socket in the session object it is
important that the data stored in the Boost buffer remains valid until
after the data is completely sent. This is why a response_builder object
is passed to the callback function whenever performing a write operation.
If this convention is not followed, then undefined behavior will probably
be noticed.

`connection.cc`:

* The connection uses the Boost
[Asio](https://www.boost.org/doc/libs/1_66_0/doc/html/boost_asio.html)
library to send data stored in a Boost
[buffer](https://www.boost.org/doc/libs/1_50_0/doc/html/boost_asio/reference/buffer.html)
to a socket using TCP. The tcp_connection class is derived from a abstract
base class connection so that other possible types of connections can be
used instead of TCP.

`request_parser.cc`:

* The request parser parses an HTTP request and populates the fields of the
request object with all the relevant information.

`response.cc`:

* This file contains the implementations of all the methods of
`response_builder` which are used to create a response by a request
handler by using some convenient functions like the `add_header()` function.
Since the response object does not have any methods, this file only
relates to the builder for a response.

`config_parser.cc`:

* Parse the config file into `location_parse_result`s.

`log_helper.cc`

* Provides useful functions which use the Boost logging library to log
certain information about the server during runtime.

`echo_request_handler.cc`:

* Implementation of the echo request handler.

`static_request_handler.cc`:

* Implementation of the static request handler. This derives from the echo
request handler.

`status_request_handler.cc`:

* Implementation of the status request handler.

`not_found_request_handler.cc`:

* Implementation of the 404 handler.


## Usage
### Build
#### Local Environment
To build the main executable we will create a new directory at the top level of
the project called *build* which will be where the build is performed. Then use
cmake and make to build the project:
```bash
mkdir build
cd build
cmake ..
make
```
This will create the build directory, change your directory to the build folder,
create all necessary build files using cmake, and then perform the build with
make. To build the coverage instrumented build, create a new top level directory
for the build called *build_coverage* and then execute the following commands as
also explained
[here](https://www.cs130.org/guides/cmake/#test-code-coverage-reports).
```bash
mkdir build_coverage
cd build_coverage
cmake -DCMAKE_BUILD_TYPE=Coverage ..
make coverage
```

#### Docker
Refer to the CS130 Docker [guide](https://www.cs130.org/guides/docker/) to setup
Docker in your local development environment and for the basics of building and
running Docker containers. The below information just covers the same
information but specifically in the context of running the WNZA server.

To build the Docker container, the base image must first be built and then the
main image can be built. To build the base image, in the top most directory
build the environment specified in `docker/base.Dockerfile`:
```bash
docker build -f docker/base.Dockerfile -t wnza:base .
```
Now we can build the main Docker image which is specified by
`docker/Dockerfile`:
```bash
docker build -f docker/Dockerfile -t wnza:latest .
```
Similarly, to build the coverage instrumented build perform the following:
```bash
docker build -f docker/coverage.Dockerfile -t wnza:coverage .
```

### Test
#### Local Environment
Our integration test depends on the environment variable `PATH_TO_BIN` being
set, so before running any tests remember to set this environment variable. When
building and testing with Docker this is already handled by the Dockerfile. To set
`PATH_TO_BIN` run the following command in your development environment where
the tests are being performed:
```bash
export PATH_TO_BIN=../build/bin/server_executable
```
When performing the coverage instrumented build, we will have to change this
environment variable. We recommend that you just build and test with Docker if
you do not want to deal with setting this variable.

All of our tests are incorporated into the build system, so to run all the tests
at once just run `make test` when your current directory is the build
directory.

To run tests with the coverage build and calculate the test coverage,
first change the `PATH_TO_BIN` environment variable to the location of the the
coverage build by executing the following in your developement environment:
```bash
export PATH_TO_BIN=../build_coverage/bin/server_executable
```
Then follow the build instructions in the [build section](#build) relating to
the coverage build, then run `make coverage` when inside the build_coverage
directory.

#### Docker
To test with Docker, just follow the above build instructions for the Docker
container which will perform both the build and run all the tests.

### Run
#### Local Environment
To run the server in a local development environment the directory which the
server is run from will matter due to the use of relative paths in the way that
the server is configured. This means that to use our existing config files, the
server must be run from the /tests directory. To run the server, use the
following commands:
```bash
cd /WNZA/tests
$PATH_TO_BIN test_server_config &
```
This runs the server as a background process. To kill the process simply execute
`fg` to bring the process into the foreground and then kill it with control C.

#### Docker
To run the Docker image, use the following command:
```bash
docker run --publish 8080:80 wnza:latest
```
The server listens on port 80 in the docker container, so this command with map
port 80 in the container to port 8080 in the local environment. This allows the
use of `nc` and even testing with a browser by just using `localhost:8080` as
the host name.

## Adding a new request_handler
To add a new request handler, we create a new class which inherits the
request_handler public interface and overrides the virtual handle_request(.)
method. Inside the handle_request(.) method, a response_builder object will be
created and certain functions of the response_builder are called in order to
craft the specific response which is wanted. Then the build_response(.) method of the
response builder is used to return the corresponding response object from the
handle_request(.) function.

The following shows an excerpt from a header file for a request_handler. The
important points to note are that the class inherits from `public
request_handler` and the init() and handle_request() functions from
request_handler are overwritten. These three points are the only requirements
necessary to building a new request_handler which can be used with the server.
The arguments to the request handler's init function are the scoped
`NginxConfig` (everything between the curly brackets of a location directive)
and the string location_path. The `NginxConfig` can be used to pass arguments
directly to a request handler based on statements that are placed inside the
location directive. The location_path comes from the URI prefix which a request
was matched to before being dispatched to this handler. This information is
important for a handler which needs to know how to correctly interpret the
`uri_` field of a request.
```c++
class echo_request_handler : public request_handler
{
public:
    static request_handler* init(const std::string& location_path,
                                 const NginxConfig& config);
    response handle_request(const request& req);
};
```

For the implementation of the init method, we create an echo_request_handler on
the heap and then return the pointer to it. We manage this pointer with a
`unique_ptr` after it is returned from init.
```c++
request_handler* echo_request_handler::init(const NginxConfig& config)
{
    return new echo_request_handler();
}
```

The handle_request method is where the construction of the response object takes
place. The response is constructed by using a response_builder object and using
the methods set_code(), add_header(), and add_body() to construct the response
from fields of the request object which is passed as an argument to the
function.
```c++
response echo_request_handler::handle_request(const request& req)
{
    response_builder res;  // Provides a convenient API for building a response
    if (req.method_ != request::INVALID) {
        std::string methods[] = {"GET", "POST", "PUT", "HEAD"};
        std::string body;

        // The set_code function is used to set HTTP response codes.
        // The options are: BAD_REQ, NOT_FOUND, OK
        res.set_code(response::status_code::OK);

        // The add_header(name, val) function is used to add a header to the
        // response object.
        res.add_header("Content-Type", "text/plain");

        // The following code is what makes this handler the echo handler. The
        // body of the response is created by reconstructing the request from the
        // fields in the request object.
        body += methods[req.method_] + " " + req.uri_ + " " + req.version_ + "\r\n";
        for (auto&& entry : req.headers_)
            body += entry.first + ": " + entry.second + "\r\n";
        body += "\r\n";
        body += req.body_;

        // Once the body string is completely generated, add as the body of the
        // response by using the add_body function of the builder.
        res.add_body(body);
        res.add_header("Content-Length", std::to_string(body.length()));
    } else
        res.make_400_error();   // Creates a 400 response with no body

    res.make_date_servername_headers();
    return res.get_response();
}
```

Finally, once the request handler is implemented, it has to be added into the
dispatch mechanism and a new keyword has to be added to the config file format
to be able to create this handler. In this example, the keyword to create an
echo handler is `EchoHandler`. Request handlers are added to the server by
creating a vector of all the "location" keywords in the config file and storing
the relavent informating from the config file in a structure called a
`location_parse_result`. Then, we can iterate through all the possible
declarations of request handlers and match the `handler_name` of the
`location_parse_result` to whatever unique keyword was assigned for creating the
specified request handlers. The following code exerpt shows how to create an
`echo_request_handler` by matching `loc_res.handler_name` to "EchoHandler" and
then simply calling the `add_request_handler` with the handler's init method and
the `loc_res` object.
```c++
std::vector<location_parse_result> location_results =
    config.get_location_result();
for (location_parse_result loc_res : location_results) {
    if (loc_res.handler_name == "EchoHandler") {
        add_request_handler(echo_request_handler::init, loc_res);
```

Now that the request handlers are instantiated based off of the config file, the
dispatch mechanism in the `session` class will direct all requests whose
`uri_` field has the URI prefix from the location directive in the
config as a prefix, to the specified request handler based on the unique token
like "EchoHandler". As an example, we will use the following config file.
```
port 80;
location "/echo/" EchoHandler {}
```
This config file will cause an `echo_request_handler` to be created and will
get all requests with the prefix "/echo/". The dispatcher uses longest prefix
match, so even if a request has the prefix of a certain handler, there may exist
another handler defined which matched the request with a longer prefix.
### Adding Logging
Our logger is a singleton so that it can be access within any class. To get an
instance of the logger reference, just place the following code somewhere in
your requests handler

```c++
log_helper& logger = log_helper::instance();
```

With this reference to the singleton logger you can use any of the functions
implemented by the `log_helper` class.


## Load testing
### Run Locust testing script
You should install Locust python3 module first and go to tests/, and
run the script load_tester.py
```python
pip3 install locust
locust -f ./load_tester.py --host=http://localhost:8080 --headless -r 100 -u 500 --csv=req
```

You can also use the browser UI to run Locust by using
```
locust -f ./load_tester.py --host=http://localhost:8080
```
Open your browser, and type in http://localhost:8089, 
you can see the UI to control your locust tester.
Here --host is the destination you want to test. 
If you want to test the server on the cloud, change it to:
```python
locust -f ./load_tester.py --host=http://www.wnza.cs130.org
```

### A graph of RPS over time
This graph can be drived directly in the locust UI. Just click Charts, 
you will see the Totals Requests per Second chart as we need.

### A graph of response time for the echo endpoint, a large static file, 
### and the status page over time
In order to draw this graph, just click Downloaded Data in locust UI, and
download requests statistics CSV, and change name to requests_data.csv.
Finally run draw_graph.py in tests/ to get a graph of response time for three 
types of endpoints.
```python
python3 draw_graph.py
```

If you want to get response time of all endpoints, you can also go to locust UI,
click graph, and check Response Time (ms) for details.
If you still want to get response time over time graph on each endpoint,
you can run load_tester_echo, load_tester_status, load_tester_static respectively.
```python
locust -f ./load_tester_echo.py --host=http://localhost:8080
locust -f ./load_tester_status.py --host=http://localhost:8080
locust -f ./load_tester_static.py --host=http://localhost:8080
```

### A graph of the CPU usage of the virtual machine on Google Compute Engine
If you choose to test the server on the cloud, go to 
https://console.cloud.google.com/monitoring/dashboards, click WNZA at the bottom,
you will see the graph of CPU utilization of VM on GCP over time.


