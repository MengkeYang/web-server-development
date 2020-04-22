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

# update log_helper as one library
add_library(log_helper_lib src/log_helper.cc)
# update config_parser as one library
add_library(config_parser_lib src/config_parser.cc)
# update request_parser as one library
add_library(request_parser_lib src/request_parser.cc)
# update session as one library
add_library(session_lib src/session.cc)
# update server as one library
add_library(server_lib src/server.cc)
add_library(response_lib src/response.cc)
add_library(connection_lib src/connection.cc)
target_link_libraries(session_lib request_parser_lib response_lib connection_lib)
target_link_libraries(server_lib session_lib)
# update main executable and link it with boost, session_lib and server_lib
add_executable(server_executable src/server_main.cc)
target_link_libraries(server_executable server_lib response_lib config_parser_lib connection_lib pthread log_helper_lib Boost::system Boost::regex Boost::log_setup Boost::log)

# update request parser test executable name, srcs, and deps
add_executable(request_parser_lib_test tests/request_parser_test.cc)
target_link_libraries(request_parser_lib_test request_parser_lib gtest_main)
# update config parser test executable name, srcs, and deps
add_executable(config_parser_lib_test tests/config_parser_test.cc)
target_link_libraries(config_parser_lib_test config_parser_lib gtest_main)
# update response test executable name, srcs, and deps
add_executable(response_lib_test tests/response_lib_test.cc)
target_link_libraries(response_lib_test Boost::system response_lib gtest_main)
add_executable(session_lib_test tests/session_lib_test.cc tests/fake_connection.cc)
target_link_libraries(session_lib_test Boost::system session_lib gtest_main)
add_test(NAME integration_test COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_test.sh WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)

# Update with test binary
gtest_discover_tests(request_parser_lib_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(config_parser_lib_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(response_lib_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(session_lib_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)

# Update with target/test targets
include(cmake/CodeCoverageReportConfig.cmake)
generate_coverage_report(TARGETS server_executable server_lib session_lib response_lib config_parser_lib request_parser_lib connection_lib TESTS request_parser_lib_test config_parser_lib_test response_lib_test session_lib_test)

