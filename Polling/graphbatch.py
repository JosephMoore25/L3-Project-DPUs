import sys
import numpy as np
#import matplotlib as plt
import matplotlib.pyplot as plt
import math

plt.rc("axes", titlesize=14)
plt.rc("axes", labelsize=14)

plt.rc("xtick", labelsize=8)
plt.rc("ytick", labelsize=12)

plt.gcf().set_size_inches(8, 6)

Ns = [0.0002, 0.0004, 0.0008, 0.0012, 0.0016, 0.0024, 0.0032, 0.0048, 0.0064, 0.0128, 0.0256, 0.0512, 0.1024, 0.2048]
def grabdata(arch):
    timept = []
    offloaded = []
    for i in range(0, len(Ns)):
        if (arch=="host"):
            filename = "./batchdata/baseline" + str(Ns[i]) + "fourcommcores.txt"
        else:
            filename = "./batchdata/bfdtask" + str(Ns[i]) + ".txt"
        datafile = open(filename, 'r')
        Lines = datafile.readlines()
        stats = Lines[0].split()

        num_tasks = int(stats[2])
        task_size = int(stats[8])
        timepertask = float(stats[12])
        tasks_per_enclave = int(stats[16])

        times = []
        offloaded_tasks = []

        for line in Lines[1:-1]:
            templine = line.strip().split()
            offloaded_tasks.append(int(templine[2]))
            times.append(float(templine[4]))
        
        timept.append(times)
        offloaded.append(offloaded_tasks)

    offloaded = offloaded[0]
    min_times = []
    percentage_of_original = []
    best_percentage_offloaded = []
    for j in range(0, len(timept)):
        min_times.append(min(timept[j][1:]))
        percentage_of_original.append((min_times[-1] / timept[j][0] - 1) * -100)
        best_percentage_offloaded.append(offloaded[timept[j].index(min(timept[j]))] / offloaded[-1])
    return Ns, min_times, percentage_of_original, best_percentage_offloaded

_, min_times2, percentage_of_original2, best_percentage_offloaded2 = grabdata("bfd")
Ns, min_times, percentage_of_original, best_percentage_offloaded = grabdata("host")

print(percentage_of_original)
print(percentage_of_original2)


#print(Ns)
#print(min_times)
#print(percentage_of_original)
#print(best_percentage_offloaded)
log_labels = []
for i in range(0, len(Ns)):
    log_labels.append(math.log2(Ns[i]))
    #Ns[i] = str(Ns[i])
    

plt.plot(log_labels, percentage_of_original2, marker="x", color="C0", markerfacecolor="b", markeredgecolor="b")
plt.plot(log_labels, percentage_of_original, marker="^", color="C1", markerfacecolor="orangered", markeredgecolor="orangered")

plt.bar(log_labels[:2], percentage_of_original2[:2], align="center", width=0.3, color="C0")#, width=0.01)

plt.bar(log_labels[:10], percentage_of_original[:10], align="center", width=0.3, color="C1")#, width=0.01)

plt.bar(log_labels[2:], percentage_of_original2[2:], align="center", width=0.3, color="C0")#, width=0.01)


plt.bar(log_labels[10:], percentage_of_original[10:], align="center", width=0.3, color="C1")#, width=0.01)


plt.axhline(y=0, color="k", linewidth=.8)
plt.xlabel("Time per task / s")
plt.ylabel("Percentage speedup vs 0 tasks offloaded")
plt.xticks(log_labels, Ns)
#plt.xticklabels(Ns)
#plt.xscale("log", base=2)

plt.ylim(ymax = 60, ymin = -60)

plt.setp(plt.gca().get_xticklabels(), rotation=30, horizontalalignment='right')

#plt.gca().yaxis.set_minor_formatter(mticker.ScalarFormatter())
plt.tight_layout()

plt.legend(["DPUs", "Hosts"])

plt.savefig("./plots/hostspeedups.png")