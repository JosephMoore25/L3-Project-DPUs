#include <iostream>
#include <mpi.h>


//Our tester struct to use
struct tester {
	int i[3];
	double d[2];
};

MPI_Datatype test_type() {
	//Define what native types are to be used
	MPI_Datatype types[] = {MPI_INT, MPI_DOUBLE};
	//Define block lengths for each var
	int blocklengths[] = {3, 2};
	const int NumAttributes = 2;
	
	//Create a temp struct to calculate displacements
	tester test;
	MPI_Aint baseaddr;
	MPI_Aint displacements[2];

	//Calculate the base address for the first element + start of struct
	MPI_Get_address(&test, &baseaddr);
	MPI_Get_address(&test.i, &displacements[0]);
	displacements[0] = MPI_Aint_diff(displacements[0], baseaddr);

	//Calculate displacement to next block
	MPI_Get_address(&test.d, &displacements[1]);
	displacements[1] = MPI_Aint_diff(displacements[1], baseaddr);

	//Create our datatype using our calculated struct layout
	MPI_Datatype datatype;
	MPI_Type_create_struct(NumAttributes, blocklengths, displacements, types, &datatype);
	MPI_Type_commit(&datatype);
	return datatype;
}

int main(int argc, char** argv) {
	//Initialise MPI
	MPI_Init(&argc, &argv);
	int world_size, my_rank;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	//Define our datatype
	MPI_Datatype HostDatatype = test_type();

	//From first rank:
	if (my_rank == 0) {
		//Declare our data
		tester HostData = {
			{ 0, 1, 2},
			{ 3.0, 4.0 }
		};
		//Send from rank 1 to rank 2
		std::cout << "Sending data from host...\n";
		MPI_Send(&HostData, 1, HostDatatype, 1, 0, MPI_COMM_WORLD);
		std::cout << "Sent data from host!\n";
	}
	//Else on other rank
	else {
		//Declare empty data to plug received into
		tester RecvData = {
			{ 0, 0, 0},
			{ 0.0, 0.0}
		};
		//Receive data
                MPI_Status status;
		std::cout << "Receiving data from host...\n";
		MPI_Recv(&RecvData, 1, HostDatatype, 0, 0, MPI_COMM_WORLD, &status);
		std::cout << "Received!\n";
		std::cout << "The received tester was i: { " << RecvData.i[0] << ", " << RecvData.i[1] << ", " <<  RecvData.i[2] << " } and d: { " << RecvData.d[0] << ", " << RecvData.d[1] << " }\n";
	}
	//Finalise MPI
	MPI_Finalize();
};
