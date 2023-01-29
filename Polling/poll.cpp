#include <mpi.h>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <time.h>

int main(int argc, char **argv) {
	//Initialize MPI - needs to be done before any other MPI command
	MPI_Init(&argc, &argv);
	//The above but with some error handling
	//if (MPI_Init(&argc,&argv) != MPI_SUCCESS) MPI_Abort(MPI_COMM_WORLD, 1);
	
	//Get num processes in MPI_COMM_WORLD
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	
	//Where hosts end and bluefields begin
	int rank_boundary = world_size / 2;

	//Get the rank of this process in MPI_COMM_WORLD
	int my_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	//MPI_Status status;
	
	int len;
	char name[MPI_MAX_PROCESSOR_NAME];
	MPI_Get_processor_name(name, &len);

	unsigned int microsecond = 1000000;
	srand(time(NULL));

	//HOST CODE	
	if (my_rank < rank_boundary) {
		std::cout << "Hello from rank " << my_rank << "! I am host " << name << "\n";
		int messages_sent = 0;
		while (messages_sent < 2) {
			float random = (float) rand() / (float) rand();
			std::cout << "Waiting " << random * 3 << " seconds\n";
			usleep(random * 3 *  microsecond);
			int message = 17;
			int dest = my_rank + rank_boundary;
			MPI_Request request = MPI_REQUEST_NULL;
			MPI_Isend(&message, 1, MPI_INT, dest, 77, MPI_COMM_WORLD, &request);
			std::cout << "Sent a message!\n";
			messages_sent++;
		}
	}
	//BLUEFIELD CODE
	else {
		std::cout << "Hello from rank " << my_rank << "! I am bluefield " << name << "\n";
		int messages_recv = 0;
		MPI_Status status;
		while (messages_recv < 2) {
			std::cout << "Waiting for message...\n";
			int received_msg = 0;
			//Poll for a message
                	while (received_msg == 0) {
				MPI_Iprobe(0, 77, MPI_COMM_WORLD, &received_msg, &status);
			}
			std::cout << "Found a message!\n";
			int count;
			MPI_Get_count( &status, MPI_INT, &count );
			int recv_buf[count];
			MPI_Request request = MPI_REQUEST_NULL;
			//We know there's a message so recieve it
			MPI_Irecv(&recv_buf, count, MPI_INT, MPI_ANY_SOURCE, 77, MPI_COMM_WORLD, &request);
			std::cout << "Recieved a message!\n";
			messages_recv++;			
		}
	}
	//Print info about MPI_COMM_WORLD
	//std::cout << "Hello World from Rank " << my_rank << " of " << world_size << "!" << "This is from: " << name << "\n";
	//Finalize MPI - must be called after all MPI functions
	MPI_Finalize();

	return 0;
}
