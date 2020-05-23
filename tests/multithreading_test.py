#!/usr/bin/env python3

import socket

HOST = '127.0.0.1'  # The server's hostname or IP address
PORT = 8080         # The port used by the server

s1 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s2 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s3 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s4 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Send a half request into socket s1
s1.connect((HOST, PORT))
req1 = "GET /echo HTTP/1.1\r\nContent-Length: 10\r\n\r\n"
s1.send(req1.encode())

# Send a full request into socket s2, expect result
s2.connect((HOST, PORT))
req2 = "GET /echo HTTP/1.1\r\n\r\n"
s2.send(req2.encode())
response = s2.recv(4096);
result = repr(response)
n = 1
if result.startswith("b'HTTP/1.1 200 OK"):
    print("Passed Test", n);
    n += 1
else:
    print("Failed Test", n)
    exit()

# Now complete the first request by sending the rest of the request, expect a
# result
req1_end = "0123456789"
s1.send(req1_end.encode())
response = s1.recv(4096)
result = repr(response)
if result.startswith("b'HTTP/1.1 200 OK"):
    print("Passed Test", n)
    n += 1
else:
    print("Failed Test", n)
    exit()
