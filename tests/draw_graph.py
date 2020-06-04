## a graph of response time for the echo endpoint, a large static file, and the status page over time
#  draw this graph from downloaded requests_data.csv:

import csv
import os
from os import path
import matplotlib.pyplot as plt

if not path.exists("requests_data.csv"):
    print(" requests_data.csv not found! ")

x_axis = []
echo_res = []
large_res = []
status_res = []
with open('requests_data.csv') as csvfile:
    reader = csv.reader(csvfile, delimiter=',')
    for row in reader:
        if 'Name' in row[1]:
            x_axis = row[11:]
        elif 'echo' in row[1]:
            echo_res = [float(x) for x in row[11:]]
        elif 'static' in row[1]:
            large_res = [float(x) for x in row[11:]]
        elif 'status' in row[1]:
            status_res = [float(x) for x in row[11:]]

plt.plot(x_axis, echo_res)
plt.plot(x_axis, large_res)
plt.plot(x_axis, status_res)
plt.xlabel('percent of users requesting')
plt.ylabel('response time (ms)')
plt.show()





