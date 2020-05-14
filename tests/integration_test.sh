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

## Test the echo request handler
output=$(curl -s localhost:8080/echo)
expected=$(cat integration_response1)
diff -q <(sort <(echo "$output")) <(sort <(echo "$expected"))
if [ $? -eq 0 ]
then
    echo "Passed Test 4"
else
    echo "Failed Test 4"
    kill -TERM $SERVER_ID
    exit 1
fi

## Testing static request handler by matching output to actual content
tmpfile=$(mktemp)
curl -s -o "$tmpfile" localhost:8080/static/cat.jpg
diff -q <(cat cat.jpg) <(cat "$tmpfile")
if [ $? -eq 0 ]
then
    echo "Passed Test 5"
else
    echo "Failed Test 5"
    kill -TERM $SERVER_ID
    rm "$tmpfile"
    exit 1
fi

## Testing 404 handler by ensuring the "/" prefix gets the 404 page
curl -s -o "$tmpfile" localhost:8080/
diff -q <(cat 404page.html) <(cat "$tmpfile")
if [ $? -eq 0 ]
then
    echo "Passed Test 6"
else
    echo "Failed Test 6"
    kill -TERM $SERVER_ID
    rm "$tmpfile"
    exit 1
fi

kill -TERM $SERVER_ID
rm "$tmpfile"
exit 0
