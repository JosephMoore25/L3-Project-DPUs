#include <mpi.h>
#include <iostream>
#include <chrono>

#include "bfd_offload.h"


int main(int argc, char **argv) {
	MPI_Init(&argc, &argv);
	//Get num processes in MPI_COMM_WORLD
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	int my_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	int len;
	char name[MPI_MAX_PROCESSOR_NAME];
	MPI_Get_processor_name(name, &len);
	
	switch(my_rank) {
		//First host to send message to its own bluefield (rank 2)
		case 0:
		{
			//std::chrono::high_resolution_clock::time_point tstart = std::chrono::high_resolution_clock::now();

			//Send message to bfd, rank 2
			int message_len = 1;
			int message[message_len] = {17};
			MPI_Request req = bfdoffload::iSendBlock(message_len, message, 2, 0);
			std::cout << "Host 1 sent a message!\n";
			MPI_Wait(&req, MPI_STATUS_IGNORE);


			//Receive message from bfd 1, rank 2
			int count = bfdoffload::Poll(2, 5);
			int *recv_buf;
			recv_buf = bfdoffload::RecvBlock(2, count, 5);
            std::cout << "Host 1 received a message from Bluefield 1!\n";

			//std::chrono::high_resolution_clock::time_point tend = std::chrono::high_resolution_clock::now();
			//std::chrono::duration<double> run_time = std::chrono::duration_cast<std::chrono::duration<double>>(tend - tstart);

			//std::cout << "Completed full trip in: " << run_time.count() << " seconds\n";
			break;
		}


		case 1:
		{

			//Poll for message from its own bluefield (this rank can be seen as an idle rank)
			int count = bfdoffload::Poll(3, 2);
			int *recv_buf;
			recv_buf = bfdoffload::RecvBlock(3, count, 2);
			std::cout << "Host 2 received a message from Bluefield 2!\n";

			MPI_Request req = bfdoffload::iSendBlock(count, recv_buf, 3, 3);
            //std::cout << "Host 2 sent a message back to Bluefield 2!\n";
            MPI_Wait(&req, MPI_STATUS_IGNORE);


			break;
		}	


		case 2:
		{
			//Poll for message from host
			int count = bfdoffload::Poll(0, 0);
			int *recv_buf;
			recv_buf = bfdoffload::RecvBlock(0, count, 0);
			std::cout << "Bluefield 1 received a message from Host 1!\n";
				


			//Pass message onto Bluefield 2 (rank 3)
			MPI_Request req = bfdoffload::iSendBlock(count, recv_buf, 3, 1);
			MPI_Wait(&req, MPI_STATUS_IGNORE);

			//Receive message from Bluefield 2
			int count2 = bfdoffload::Poll(3, 4);
			int *recv_buf2;
			recv_buf2 = bfdoffload::RecvBlock(3, count2, 4);
            std::cout << "Bluefield 1 received a message from Bluefield 2!\n";

			//Pass message back to Host 1
			MPI_Request req2 = bfdoffload::iSendBlock(count2, recv_buf2, 0, 5);
            MPI_Wait(&req2, MPI_STATUS_IGNORE);


			break;
		}


		case 3:
		{

			//Poll for message from Bluefield 1
			int count = bfdoffload::Poll(2, 1);
			int *recv_buf;
			recv_buf = bfdoffload::RecvBlock(2, count, 1);
			std::cout << "Bluefield 2 received a message from Bluefield 1!\n";
			
			//Pass on message to Host 2
			MPI_Request req = bfdoffload::iSendBlock(count, recv_buf, 1, 2);
			MPI_Wait(&req, MPI_STATUS_IGNORE);
			
			//Poll to receive back from Host 2
			int count2 = bfdoffload::Poll(1, 3);
			int *recv_buf2;
			recv_buf2 = bfdoffload::RecvBlock(1, count2, 3);
            std::cout << "Bluefield 2 received a message from Host 2!\n";

			//Pass this message to Bfd 1
            MPI_Request req2 = bfdoffload::iSendBlock(count2, recv_buf2, 2, 4);
            MPI_Wait(&req2, MPI_STATUS_IGNORE);


			break;
		}
	}


	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();

	return 0;
}
