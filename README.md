# WNZA HTTP Server

## Source Layout
Our directory structure separates header files from their corresponding
implementation files. All implementation files (.cc) are placed into the src/
directory and all header files (.h) are placed into the include/ directory. All
tests and files necessary for the tests are located in the tests/ directory. We
also have dockerfiles placed in the docker/ directory. Finally, output log files
are stored in the log/ directory.

## Usage
### Build
#### Local Environment
To build the main executable create a new directory at the top level of the
project called "build" which will be where the build is performed. Then perform
the following commands:
```bash
cd build
cmake ..
make
```
This will change your directory to the build folder, create all necessary build
files using cmake, and then perform the build with make.

#### Docker
To build the Docker container, the base image must first be built and then the
main image can be built. To build the base image, in the top most directory
perform the following:
```bash
docker build -f docker/base.Dockerfile -t wnza:base .
```
Now we can build the main Docker image by performing the following command:
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
building a testing with Docker this is already handled by the Dockerfile. To set
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
directory. To run tests with the coverage build and calculate the test coverage,
follow the build instructions above for the coverage build, then run `make
coverage` when inside the build_coverage directory.

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
```c++
class echo_request_handler : public request_handler
{
public:
    static std::unique_ptr<request_handler> init(const NginxConfig& config);
    response handle_request(const request& req);
};
```

For the implementation of the init method, we create an echo_request_handler on
the heap and then return the pointer to it from the init method.
```c++
request_handler* echo_request_handler::init(const NginxConfig& config)
{
    echo_request_handler* er = new echo_request_handler();
    return er;
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
    response_builder res;   // Builds the response with its own functions
    if (req.method_ != request::INVALID) {
        std::string methods[] = {"GET", "POST", "PUT", "HEAD"};
        std::string body;

        res.set_code(response::status_code::OK);
        res.add_header("Content-Type", "text/plain");

        body += methods[req.method_] + " " + req.uri_ + " " + "HTTP/1.1\r\n";
        for (auto&& entry : req.headers_)
            body += entry.first + ": " + entry.second + "\r\n";
        body += "\r\n";
        body += req.body_;
        res.add_header("Content-Length", std::to_string(body.length()));
        res.add_body(body);
    } else
        res.make_400_error();

    res.make_date_servername_headers(); // Add Date and Servername headers
    return res.get_response();
}
```
