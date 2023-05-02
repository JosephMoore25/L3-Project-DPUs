import sys
import numpy as np
#import matplotlib as plt
import matplotlib.pyplot as plt

plt.rc("axes", titlesize=14)
plt.rc("axes", labelsize=14)

plt.rc("xtick", labelsize=12)
plt.rc("ytick", labelsize=12)

reduced_filename = sys.argv[1].split("/")[-1][:-4]

def get_comms_time(filename):

    datafile = open(filename, 'r')
    Lines = datafile.readlines()

    Message_Size = []
    ISend = []
    Poll = []
    IRecv = []
    for line in Lines[0:-1]:
        templine = line.strip().split()
        Message_Size.append(int(templine[2]))
        ISend.append(float(templine[4]))
        Poll.append(float(templine[6]))
        IRecv.append(float(templine[8]))
        #print(line.strip().split())
    #print(Message_Size)
    #print(ISend)
    #print(Poll)
    #print(IRecv)

    comms_time = []

    for i in range(0, len(ISend)):
        comms_time.append(ISend[i]+Poll[i]+IRecv[i])
    
    return comms_time

alltimes = []
for i in range(len(sys.argv) - 1):
    filename = sys.argv[1 + i]
    alltimes.append(get_comms_time(filename))

print(alltimes)

message_sizes = []

for i in range(0, 23):
    message_sizes.append((2**i))

markers = ["x", "^", "o"]
for i in range(len(alltimes)):
    plt.semilogx(message_sizes, alltimes[i], marker=markers[i], base=2)
#plt.xscale("log", base=2)
#plt.plot(Message_Size, comms_time, marker="x")
#plt.plot(perc_tasks, peak_time, marker="x")
plt.xlabel("Message Size / B")
plt.ylabel("Time / s")

plt.legend(["2 Host Baseline", "4 Bluefields model", "4 Hosts model"])

plt.tight_layout()

#plt.title("Latency of moving messages of size n back and forth between two worker nodes via two communication nodes pt2pt", wrap=True)
plt.savefig("./../plots/" + reduced_filename + ".png")
