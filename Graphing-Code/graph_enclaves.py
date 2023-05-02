import sys
import numpy as np
#import matplotlib as plt
import matplotlib.pyplot as plt

plt.rc("axes", titlesize=14)
plt.rc("axes", labelsize=14)

plt.rc("xtick", labelsize=12)
plt.rc("ytick", labelsize=12)

plt.gcf().set_size_inches(10, 8)

#Change with varying enclave sizes.
enclaves = [1, 4, 8, 16, 32, 64]
markers = ["x", "^", "o", "s", "d", "p"]

for i in range(0, len(enclaves)):
    filename = "./../data/enclavedata/bfdtask" + str(enclaves[i]) + ".txt"
    datafile = open(filename, 'r')
    Lines = datafile.readlines()

    stats = Lines[0].split()
    num_tasks = int(stats[2])
    task_size = int(stats[8])
    time_per_task = float(stats[12])
    tasks_per_enclave = int(stats[16])

    offloaded_tasks = []
    time = []
    comms_time = []
    for line in Lines[1:-1]:
        templine = line.strip().split()
        offloaded_tasks.append(int(templine[2]))
        time.append(float(templine[4]))
        comms_time.append(float(templine[7]))
        #print(line.strip().split())
    
    perc_tasks = [j*100/num_tasks for j in offloaded_tasks]

    plt.semilogy(perc_tasks, time, marker=markers[i])
    if (i == len(enclaves)-1):
        peak_time = [time[0] * max(j, 100-j)/100 for j in perc_tasks]
        plt.semilogy(perc_tasks, peak_time, marker="*", base=2)

import matplotlib.ticker as mticker
from matplotlib.ticker import FuncFormatter

plt.gca().yaxis.set_major_formatter(FuncFormatter(lambda y, _: '{:.16g}'.format(y)))
#plt.tight_layout()

#print("Num Tasks: ", num_tasks)
#print("Task Size: " , task_size)
#print("Time Per Task: ", time_per_task)
#print("Tasks Per Enclave: ", tasks_per_enclave)


#plt.plot(perc_tasks, peak_time, marker="x")
plt.xlabel("Percentage of tasks offloaded")
plt.ylabel("Time / s")

plt.legend(["E = 1", "E = 4", "E = 8", "E = 16", "E = 32", "E = 64","Theoretical Peak Time"])

#plt.title("Offloading " + str(num_tasks) + " tasks with size " + str(task_size) + \
#" and "  + str(time_per_task) + "s per task, " + str(tasks_per_enclave) + " tasks \
#per enclave", wrap=True)
plt.savefig("./../plots/enclaves.png")
