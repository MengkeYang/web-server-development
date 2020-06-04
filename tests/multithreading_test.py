#!/usr/bin/env python3

import socket

HOST = '127.0.0.1'  # The server's hostname or IP address
PORT = 8080         # The port used by the server
NUM_THREAD = 16     # Number of thread

n=1
s_i = []
for i in range(NUM_THREAD):
    s  = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s_i.append(s)

for i in range(NUM_THREAD):
    s_i[i].connect((HOST, PORT))
    req = "GET /echo HTTP/1.1\r\nContent-Length: 10\r\n\r\n"
    s_i[i].send(req.encode())

for i in range(NUM_THREAD):
    req_end = "0123456789"
    s_i[i].send(req_end.encode())
    response = s_i[i].recv(4096)
    result = repr(response)
    if result.startswith("b'HTTP/1.1 200 OK"):
        print("Passed Test", n)
        n += 1
    else:
        print("Failed Test", n)
        exit()
