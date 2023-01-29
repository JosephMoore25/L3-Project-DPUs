#include <mpi.h>
#include <stdio.h>
#include <iostream>

int main(int argc, char **argv) {
	//Initialize MPI - needs to be done before any other MPI command
	MPI_Init(&argc, &argv);
	//The above but with some error handling
	//if (MPI_Init(&argc,&argv) != MPI_SUCCESS) MPI_Abort(MPI_COMM_WORLD, 1);
	
	//Get num processes in MPI_COMM_WORLD
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	//Get the rank of this process in MPI_COMM_WORLD
	int my_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	//Print info about MPI_COMM_WORLD
	std::cout << "Hello World from Rank " << my_rank << " of " << world_size << "!\n";

	//Finalize MPI - must be called after all MPI functions
	MPI_Finalize();

	return 0;
}
