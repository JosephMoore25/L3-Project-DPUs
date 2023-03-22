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
	bool validrun = true;

	//Set global time and time for each rank
	double tisend = 0;
	double trecv = 0;
	double tpoll = 0;
	srand (time(NULL));

	int message_len = 1;
	const int repeats = 100;
	const int message_size_modifier = 23; // 2^message_size_modifier = max message size sent

	//Make sure all vars are initialised
	MPI_Barrier(MPI_COMM_WORLD);
	for (int k=0; k<message_size_modifier; k++) {
		for (int i=0; i<repeats; i++) {
			switch(my_rank) {
				//First host to send message to its own bluefield (rank 2)
				case 0:
				{
					//std::chrono::high_resolution_clock::time_point tstart = std::chrono::high_resolution_clock::now();

					//Send message to bfd, rank 2
					int message[message_len];
					for (int j = 0; j < message_len; j++) {
						message[j] = (rand() % 10000);
					}

					std::chrono::high_resolution_clock::time_point tisendstart = std::chrono::high_resolution_clock::now();
					MPI_Request req = bfdoffload::iSendBlock(message_len, message, 2, 0);
					std::chrono::high_resolution_clock::time_point tisendend = std::chrono::high_resolution_clock::now();
					tisend += std::chrono::duration_cast<std::chrono::duration<double>>(tisendend - tisendstart).count();

					//std::cout << "Host 1 sent a message!\n";
					MPI_Wait(&req, MPI_STATUS_IGNORE);


					//Receive message from bfd 1, rank 2
					std::chrono::high_resolution_clock::time_point tpollstart = std::chrono::high_resolution_clock::now();
					int count = bfdoffload::Poll(2, 5);
					std::chrono::high_resolution_clock::time_point tpollend = std::chrono::high_resolution_clock::now();
					tpoll += std::chrono::duration_cast<std::chrono::duration<double>>(tpollend - tpollstart).count();

					int *recv_buf = new int[count];

					std::chrono::high_resolution_clock::time_point trecvstart = std::chrono::high_resolution_clock::now();
					MPI_Status recv_status;
					MPI_Recv(recv_buf, count, MPI_INT, 2, 5, MPI_COMM_WORLD, &recv_status);
					std::chrono::high_resolution_clock::time_point trecvend = std::chrono::high_resolution_clock::now();
					trecv += std::chrono::duration_cast<std::chrono::duration<double>>(trecvend - trecvstart).count();

					//std::cout << "Host 1 received a message from Bluefield 1!\n";

					//std::cout << recv_buf[0] << "\n";
					if (recv_buf[0] != message[0] + 1) {
						std::cout << "Test failed. Something's not quite right.\n";
						validrun = false;
						goto endloop;
					}

					//std::chrono::high_resolution_clock::time_point tend = std::chrono::high_resolution_clock::now();
					//std::chrono::duration<double> run_time = std::chrono::duration_cast<std::chrono::duration<double>>(tend - tstart);

					//std::cout << "Completed full trip in: " << run_time.count() << " seconds\n";
					break;
				}


				case 1:
				{

					//Poll for message from its own bluefield (this rank can be seen as an idle rank)
					int count = bfdoffload::Poll(3, 2);
					int recv_buf[count];

					MPI_Status recv_status;
					MPI_Recv(&recv_buf, count, MPI_INT, 3, 2, MPI_COMM_WORLD, &recv_status);

					//std::cout << "Host 2 received a message from Bluefield 2!\n";
					MPI_Request req = bfdoffload::iSendBlock(count, recv_buf, 3, 3);
					//std::cout << "Host 2 sent a message back to Bluefield 2!\n";
					MPI_Wait(&req, MPI_STATUS_IGNORE);


					break;
				}	


				case 2:
				{
					//Poll for message from host
					int count = bfdoffload::Poll(0, 0);
					int recv_buf[count];

					MPI_Status recv_status;
					MPI_Recv(&recv_buf, count, MPI_INT, 0, 0, MPI_COMM_WORLD, &recv_status);

					//std::cout << "Bluefield 1 received a message from Host 1!\n";

					//Increment recv_buf for validation
					recv_buf[0]++;
						

					//Pass message onto Bluefield 2 (rank 3)
					MPI_Request req = bfdoffload::iSendBlock(count, recv_buf, 3, 1);
					MPI_Wait(&req, MPI_STATUS_IGNORE);

					//Receive message from Bluefield 2
					int count2 = bfdoffload::Poll(3, 4);
					int recv_buf2[count2];

					MPI_Status recv_status2;
					MPI_Recv(&recv_buf2, count2, MPI_INT, 3, 4, MPI_COMM_WORLD, &recv_status2);

					//std::cout << "Bluefield 1 received a message from Bluefield 2!\n";

					//Pass message back to Host 1
					MPI_Request req2 = bfdoffload::iSendBlock(count2, recv_buf2, 0, 5);
					MPI_Wait(&req2, MPI_STATUS_IGNORE);


					break;
				}


				case 3:
				{

					//Poll for message from Bluefield 1
					int count = bfdoffload::Poll(2, 1);
					int recv_buf[count];

					MPI_Status recv_status;
					MPI_Recv(&recv_buf, count, MPI_INT, 2, 1, MPI_COMM_WORLD, &recv_status);

					//std::cout << "Bluefield 2 received a message from Bluefield 1!\n";
					
					//Pass on message to Host 2
					MPI_Request req = bfdoffload::iSendBlock(count, recv_buf, 1, 2);
					MPI_Wait(&req, MPI_STATUS_IGNORE);
					
					//Poll to receive back from Host 2
					int count2 = bfdoffload::Poll(1, 3);
					int recv_buf2[count2];

					MPI_Status recv_status2;
					MPI_Recv(&recv_buf2, count2, MPI_INT, 1, 3, MPI_COMM_WORLD, &recv_status2);
					//std::cout << "Bluefield 2 received a message from Host 2!\n";

					//Pass this message to Bfd 1
					MPI_Request req2 = bfdoffload::iSendBlock(count2, recv_buf2, 2, 4);
					MPI_Wait(&req2, MPI_STATUS_IGNORE);


					break;
				}
			}
			//message_len *= 2;
			MPI_Barrier(MPI_COMM_WORLD);
		}
		if (my_rank == 0) {
			if (validrun == true) {
				std::cout << "Message Size: " << message_len << "   ISend: " << tisend/repeats << "   Poll: " << tpoll/repeats << "   Recv: " << trecv/repeats << "\n";
			}
			else { break; }
		}
		message_len *= 2;
	}

endloop:
	if (my_rank == 0) {
		if (validrun == true) {
			std::cout << "Test passed. Result valid\n";
		}
		else {
			std::cout << "Test failed. Something's not quite right.\n";	
		}
	}

	MPI_Finalize();

	return 0;
}
