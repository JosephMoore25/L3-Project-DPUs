#include <mpi.h>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <time.h>

int main(int argc, char **argv) {
	//Initialize MPI
	MPI_Init(&argc, &argv);
	
	//Get num processes in MPI_COMM_WORLD
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	
	//Where hosts end and bluefields begin
	int rank_boundary = world_size / 2;

	//Get the rank of this process in MPI_COMM_WORLD
	int my_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	
	int len;
	//We want a string to know the process hostname
	char name[MPI_MAX_PROCESSOR_NAME];
	MPI_Get_processor_name(name, &len);
	
	//Init random seed
	srand(time(NULL));

	//HOST CODE	
	if (my_rank < rank_boundary) {
		std::cout << "Hello from rank " << my_rank << "! I am host " << name << "\n";
		int messages_sent = 0;
		while (messages_sent < 3) {
			float random = (float) rand() / (float) rand();
			std::cout << "Waiting " << random * 3 << " seconds\n";
			usleep(random * 3 * 1000000);

			int message_len = 1;
			int message[message_len] = {17};
			
			//Send to its own bluefield
			int dest = my_rank + rank_boundary;
			MPI_Request request = MPI_REQUEST_NULL;
			MPI_Isend(&message, message_len, MPI_INT, dest, 77, MPI_COMM_WORLD, &request);
			std::cout << "Sent a message!\n";
			messages_sent++;
		}
	}
	//BLUEFIELD CODE
	else {
		std::cout << "Hello from rank " << my_rank << "! I am bluefield " << name << "\n";
		int messages_recv = 0;
		MPI_Status status;
		while (messages_recv < 3) {
			std::cout << "Waiting for message...\n";
			int received_msg = 0;
			//Poll for a message
                	while (received_msg == 0) {
				MPI_Iprobe(MPI_ANY_SOURCE, 77, MPI_COMM_WORLD, &received_msg, &status);
			}
			std::cout << "Found a message!\n";
			//Get message length
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
	//Finalize MPI
	MPI_Finalize();

	return 0;
}
