#include <iostream>
#include <mpi.h>

int main(int argc, char **argv) {
	//Initialise MPI
	int world_size, my_rank;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	int N = 200000000;

	//Init workloads for num ranks
	int workloads[world_size];

	//For each rank, the workload = N/num ranks. Fill in any remainders
	for (int i=0; i<world_size; i++) {
		workloads[i] = N / world_size;
		if (i < N % world_size ) { workloads[i]++; }
	}

	//Figure out how many N we'll need to compute
	int my_start = 0;
	for (int i=0; i<my_rank; i++) {
		my_start += workloads[i];
	}
	int my_end = my_start + workloads[my_rank];

	//Time this
	double start_time = MPI_Wtime();

	//Create an answer array, only as big as our workload to save memory
	double *a = new double[workloads[my_rank]];
	double sum = 0;

	//Only do the work for our workload, and then create a local sum var to collect it all for our sub-problem
	for (int i = 0; i < workloads[my_rank]; i++) {
		a[i] = 1.0 + (0.2*float(i));
	}
	for (int i = 0; i < workloads[my_rank]; i++) {
		sum += a[i];
	}

	//On rank 0 collect all the partial sums to produce ans
	if (my_rank == 0) {
		for (int i=1; i<world_size; i++) {
			double partial_sum;
			MPI_Status status;
			//Recieve start of buffer pointer, amount in buffer, type, what rank from, a tag, comms, status
			MPI_Recv(&partial_sum, 1, MPI_DOUBLE, i, 77, MPI_COMM_WORLD, &status);
			sum += partial_sum;
		}
	}
	//On all other ranks, send sum to rank 1
	else {
		//Send pointer to data, how much, type, what rank to, tag, comms
		MPI_Send(&sum, 1, MPI_DOUBLE, 0, 77, MPI_COMM_WORLD);
	}

	double end_time = MPI_Wtime();

	std::cout.precision(12);
	if (my_rank == 0) {
		std::cout << "Created sequence in: " << end_time - start_time << " with ans = " << sum <<std::endl;
	}
	//Finalise MPI
	MPI_Finalize();
}
