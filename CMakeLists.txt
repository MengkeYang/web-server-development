cmake_minimum_required(VERSION 3.10.0)
project(WNZA)

# Make sure developers do not run cmake in the main project directory, to keep
# build artifacts from becoming clutter
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "In-source builds not allowed.
    Please make a new directory (called a build directory) and run CMake from there.
    You may need to remove CMakeCache.txt." )
endif()

# Turn on debug builds if we are building in a devel shell
if (CMAKE_BUILD_TYPE STREQUAL "" AND "$ENV{DEVEL_SHELL}" STREQUAL "1")
    message(STATUS "Setting debug build type by default in devel shell")
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Output binaries to a sub directory "bin"
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Enable cmake testing
include(CTest)
enable_testing()

# Enable GoogleTest
include(GoogleTest)
add_subdirectory(/usr/src/googletest googletest)

# Enable Boost
# Use static libraries so binaries can be deployed without a full boost install
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.50 REQUIRED COMPONENTS system regex log)
message(STATUS "Boost version: ${Boost_VERSION}")

include_directories(include)

# Create libraries for all class implementations
add_library(log_helper_lib src/log_helper.cc)
add_library(config_parser_lib src/config_parser.cc)
add_library(request_parser_lib src/request_parser.cc)
add_library(session_lib src/session.cc)
add_library(server_lib src/server.cc)
add_library(response_lib src/response.cc)
add_library(echo_request_handler_lib src/echo_request_handler.cc)
add_library(static_request_handler_lib src/static_request_handler.cc)
add_library(status_request_handler_lib src/status_request_handler.cc)
add_library(not_found_request_handler_lib src/not_found_request_handler.cc)
add_library(connection_lib src/connection.cc)
target_link_libraries(echo_request_handler_lib response_lib)
target_link_libraries(static_request_handler_lib response_lib not_found_request_handler_lib)
target_link_libraries(not_found_request_handler_lib response_lib)
target_link_libraries(status_request_handler_lib response_lib)
target_link_libraries(session_lib request_parser_lib response_lib connection_lib echo_request_handler_lib static_request_handler_lib status_request_handler_lib not_found_request_handler_lib log_helper_lib)
target_link_libraries(server_lib session_lib)
# update main executable and link it with boost, session_lib and server_lib
add_executable(server_executable src/server_main.cc)
target_link_libraries(server_executable server_lib config_parser_lib pthread log_helper_lib Boost::system Boost::regex Boost::log_setup Boost::log)

# Create all test executables
add_executable(request_parser_lib_test tests/request_parser_test.cc)
add_executable(response_lib_test tests/response_lib_test.cc)
add_executable(config_parser_lib_test tests/config_parser_test.cc)
add_executable(session_lib_test tests/session_lib_test.cc tests/fake_connection.cc)
add_executable(static_request_handler_lib_test tests/static_request_handler_test.cc)
add_executable(echo_request_handler_lib_test tests/echo_request_handler_test.cc)
add_executable(not_found_request_handler_lib_test tests/not_found_request_handler_test.cc)
add_executable(status_request_handler_lib_test tests/status_request_handler_test.cc)

# Link all tests
target_link_libraries(config_parser_lib_test config_parser_lib gtest_main)
target_link_libraries(request_parser_lib_test request_parser_lib gtest_main)
target_link_libraries(response_lib_test Boost::system response_lib gtest_main)
target_link_libraries(session_lib_test session_lib log_helper_lib gtest_main gmock_main Boost::system Boost::regex Boost::log_setup Boost::log config_parser_lib)
target_link_libraries(static_request_handler_lib_test Boost::system static_request_handler_lib gtest_main response_lib config_parser_lib)
target_link_libraries(echo_request_handler_lib_test Boost::system echo_request_handler_lib gtest_main response_lib config_parser_lib)
target_link_libraries(not_found_request_handler_lib_test Boost::system not_found_request_handler_lib gtest_main response_lib config_parser_lib)
target_link_libraries(status_request_handler_lib_test Boost::system Boost::log status_request_handler_lib gtest_main response_lib config_parser_lib)

add_test(NAME integration_test COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_test.sh WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)

# Update with test binary
gtest_discover_tests(request_parser_lib_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(config_parser_lib_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(response_lib_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(session_lib_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(static_request_handler_lib_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(not_found_request_handler_lib_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(status_request_handler_lib_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(echo_request_handler_lib_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)

# Update with target/test targets
include(cmake/CodeCoverageReportConfig.cmake)
generate_coverage_report(TARGETS static_request_handler_lib echo_request_handler_lib server_executable server_lib session_lib response_lib config_parser_lib request_parser_lib connection_lib TESTS static_request_handler_lib_test echo_request_handler_lib_test not_found_request_handler_lib_test status_request_handler_lib_test request_parser_lib_test config_parser_lib_test response_lib_test session_lib_test)
