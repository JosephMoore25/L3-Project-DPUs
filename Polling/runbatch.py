import os

arch = "host"
model = "baseline"
Ns = [0.0002, 0.0004, 0.0008, 0.0012, 0.0016, 0.0024, 0.0032, 0.0048, 0.0064, 0.0128, 0.0256, 0.0512, 0.1024, 0.2048]

for i in range(0, len(Ns)):
    if arch == "host":
        if model != "baseline":
            command = "mpirun -n 4 -H b101,b102,b103,b104 ./singletaskhost "+ str(Ns[i]) + "> ./batchdata/task" + str(Ns[i]) + "fourcommcores.txt"
        else:
            command = "mpirun -n 2 -H b101,b102 ./baseline "+ str(Ns[i]) + "> ./batchdata/baseline" + str(Ns[i]) + "onecommcores.txt"
    else:
        command = "mpirun -n 4 -H bluefield101,bluefield102,bluefield103,bluefield104 ./singletaskbfd "+ str(Ns[i]) + "> ./batchdata/bfdtask" + str(Ns[i]) + ".txt"
    os.system(command)
    print("Done for: " + str(Ns[i]))
