import sys
import numpy as np
#import matplotlib as plt
import matplotlib.pyplot as plt

filename = sys.argv[1]
reduced_filename = filename.split("/")[-1][:-4]

datafile = open(filename, 'r')
Lines = datafile.readlines()

stats = Lines[0].split()
num_tasks = int(stats[2])
task_size = int(stats[8])
time_per_task = float(stats[12])
tasks_per_enclave = int(stats[16])

print("Num Tasks: ", num_tasks)
print("Task Size: " , task_size)
print("Time Per Task: ", time_per_task)
print("Tasks Per Enclave: ", tasks_per_enclave)

offloaded_tasks = []
time = []
comms_time = []
for line in Lines[1:-1]:
    templine = line.strip().split()
    offloaded_tasks.append(int(templine[2]))
    time.append(float(templine[4]))
    comms_time.append(float(templine[7]))
    #print(line.strip().split())
print(offloaded_tasks)
print(time)
print(comms_time)

offloaded_tasks = np.array(offloaded_tasks)
time = np.array(time)
comms_time = np.array(comms_time)
perc_tasks = [i*100/num_tasks for i in offloaded_tasks]

peak_time = [time[0] * max(i, 100-i)/100 for i in perc_tasks]

#plt.plot(offloaded_tasks, time, marker="x")
plt.plot(perc_tasks, time, marker="x")
plt.plot(perc_tasks, peak_time, marker="x")
plt.xlabel("Percentage of tasks offloaded")
plt.ylabel("Time / s")

plt.legend(["Time", "Theoretical Peak Time"])

plt.title("Offloading " + str(num_tasks) + " tasks with size " + str(task_size) + \
" and "  + str(time_per_task) + "s per task, " + str(tasks_per_enclave) + " tasks \
per enclave", wrap=True)
plt.savefig("./plots/" + reduced_filename + ".png")
