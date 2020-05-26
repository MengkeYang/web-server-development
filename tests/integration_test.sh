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

curl -s -o "$tmpfile" localhost:8080/static/snake.html
diff -q <(cat snake.html) <(cat "$tmpfile")
if [ $? -eq 0 ]
then
    echo "Passed Test 6"
else
    echo "Failed Test 6"
    kill -TERM $SERVER_ID
    rm "$tmpfile"
    exit 1
fi

## Testing 404 handler by ensuring the "/" prefix gets the 404 page
curl -s -o "$tmpfile" localhost:8080/
diff -q <(cat 404page.html) <(cat "$tmpfile")
if [ $? -eq 0 ]
then
    echo "Passed Test 7"
else
    echo "Failed Test 7"
    kill -TERM $SERVER_ID
    rm "$tmpfile"
    exit 1
fi

## Testing the multithreaded request handling
echo "hello" | nc localhost 8080 &
test_id=$!
curl -s -o "$tmpfile" localhost:8080/static/mobydick.txt
kill -TERM $test_id
diff <(cat mobydick.txt) <(cat "$tmpfile")
if [ $? -eq 0 ]
then
    echo "Passed Test 8"
else
    echo "Failed Test 8"
    kill -TERM $SERVER_ID
    rm "$tmpfile"
    exit 1
fi

# More multithreading tests with Python sockets
./multithreading_test.py

## Setting up additional servers to test proxy
$PATH_TO_BIN test_proxy_server_config &
PROXY_SERVER_ID=$!

$PATH_TO_BIN test_proxy_server_redirect_config &
PROXY_SERVER_REDIRECT_ID=$!

sleep 1s

## Testing basic proxy server by matching with main
tmpfile=$(mktemp)
curl -s -o "$tmpfile" localhost:8081/proxy/
diff  <(cat blank.html) <(cat "$tmpfile")
if [ $? -eq 0 ]
then
    echo "Passed Test 9"
else
    echo "Failed Test 9"
    kill -TERM $PROXY_SERVER_REDIRECT_ID
    kill -TERM $PROXY_SERVER_ID
    kill -TERM $SERVER_ID
    rm "$tmpfile"
    exit 1
fi

## Testing redirect on proxy server request handling
tmpfile=$(mktemp)
curl -s -i -o "$tmpfile" --http1.0 localhost:8082/proxy2/
diff  <(cat good_method_request) <(sed 's/\r//' <(head -n 1 "$tmpfile"))
if [ $? -eq 0 ]
then
    echo "Passed Test 10"
else
    echo "Failed Test 10"
    kill -TERM $PROXY_SERVER_REDIRECT_ID
    kill -TERM $PROXY_SERVER_ID
    kill -TERM $SERVER_ID
    rm "$tmpfile"
    exit 1
fi

## Test the health request handler returns a 200
curl -I -s -o "$tmpfile" localhost:8080/health
if [[ $(cat "$tmpfile") =~ HTTP/1.1\ 200\ OK.* ]]
then
    echo "Passed Test 11"
else
    echo "Failed Test 11"
    kill -TERM $PROXY_SERVER_REDIRECT_ID
    kill -TERM $PROXY_SERVER_ID
    kill -TERM $SERVER_ID
    rm "$tmpfile"
    exit 1
fi

kill -TERM $PROXY_SERVER_REDIRECT_ID
kill -TERM $PROXY_SERVER_ID
kill -TERM $SERVER_ID
rm "$tmpfile"
exit 0
