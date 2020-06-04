## a graph of response time for the echo endpoint, a large static file, and the status page over time
## First run "locust -f load_tester.py --host=http://www.wnza.cs130.org --headless -r 100 -u 500 --csv=req"
## which will run the server with 500 connections, a hatch rate of 100 conn/s and output time history
## data to req_stats_history.csv.

import csv
import os
from os import path
import matplotlib.pyplot as plt

if not path.exists("req_stats_history.csv"):
    print(" req_stats_history.csv not found! ")

x_axis = []
res = []
found_first = False
first = 0

req_per_second = []
fail_per_second = []
with open('req_stats_history.csv') as csvfile:
    reader = csv.DictReader(csvfile, delimiter=',')
    for row in reader:
        if not found_first:
            first = int(row['Timestamp'])
            found_first = True
        x_axis.append(int(row['Timestamp'])-first)
        res.append(float(row['Total Average Response Time']))
        req_per_second.append(float(row['Requests/s']))
        fail_per_second.append(float(row['Failures/s']))

plt.plot(x_axis, res)
plt.xlabel('time (s)')
plt.ylabel('total average response time (ms)')
plt.show()

plt.plot(x_axis, req_per_second, label='requests/s')
plt.plot(x_axis, fail_per_second, label='failures/s')
plt.xlabel('time (s)')
plt.legend()
plt.show()
