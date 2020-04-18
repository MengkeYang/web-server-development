#!/bin/bash

../build_coverage/bin/server_executable ../tests/example_config &
SERVER_ID=$!
sleep 1s

## Matching the output of server response to a regex expression
output=$(curl -s localhost:8080)
if [[ "$output" =~ GET[[:space:]]/[[:space:]]HTTP/1\.1.* ]]
then
    echo "Passed Test 1"
else
    echo "Failed Test 1"
fi

## Testing that some of the content of the message is correct
output=$(curl -s -d "Hello,World!" localhost:8080)
if [[ "$output" =~ .*Content-Length:[[:space:]]12.* ]]
then
    echo "Passed Test 2"
else
    echo "Failed Test 2"
fi

kill -9 $SERVER_ID
