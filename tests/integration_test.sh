#!/bin/bash

output=$("$PATH_TO_BIN" 2>&1)
if [[ "$output" =~ Usage:.* ]]
then
    echo "Passed Test 1"
else
    echo "Failed Test 1"
    exit 1
fi

output=$("$PATH_TO_BIN" no_port_config 2>&1)
if [[ "$output" =~ .*[Pp]ort.* ]]
then
    echo "Passed Test 2"
else
    echo "Failed Test 2"
    exit 1
fi

# test not exist config file
output=$("$PATH_TO_BIN" no_exist_config 2>&1)
if [[ "$output" =~ .*failed.* ]]
then
    echo "Passed Test 3"
else
    echo "Failed Test 3"
    exit 1
fi

$PATH_TO_BIN test_server_config &
SERVER_ID=$!
sleep 1s

## Matching the output of server response to a regex expression
output=$(curl -s localhost:8080/echo)
expected=$(cat integration_response1)
diff -q <(echo "$output") <(echo "$expected")
if [ $? -eq 0 ]
then
    echo "Passed Test 4"
else
    echo "Failed Test 4"
    kill -TERM $SERVER_ID
    exit 1
fi

## Testing that some of the content of the message is correct
output=$(curl -s -d "Hello,World!" localhost:8080/echo)
if [[ "$output" =~ .*Content-Length:[[:space:]]12.* ]]
then
    echo "Passed Test 5"
else
    echo "Failed Test 5"
    kill -TERM $SERVER_ID
    exit 1
fi

kill -TERM $SERVER_ID
exit 0
