#include <mpi.h>
#include <iostream>

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
					char message[message_len];
					for (int j = 0; j < message_len; j++) {
						message[j] = ('a' + (rand() % 26));
					}

					//std::chrono::high_resolution_clock::time_point tisendstart = std::chrono::high_resolution_clock::now();
					double tisendstart, tisendend;
					tisendstart = MPI_Wtime();
					MPI_Request req = bfdoffload::iSendBlock(message_len, message, 1, 0);
					//std::chrono::high_resolution_clock::time_point tisendend = std::chrono::high_resolution_clock::now();
					tisendend = MPI_Wtime();
					//tisend += std::chrono::duration_cast<std::chrono::duration<double>>(tisendend - tisendstart).count();
					tisend += tisendend - tisendstart;

					//std::cout << "Host 1 sent a message!\n";
					MPI_Wait(&req, MPI_STATUS_IGNORE);


					//Receive message from bfd 1, rank 2
					//std::chrono::high_resolution_clock::time_point tpollstart = std::chrono::high_resolution_clock::now();
					double tpollstart, tpollend;
					tpollstart = MPI_Wtime();
					int count = bfdoffload::Poll(1, 1);
					//std::chrono::high_resolution_clock::time_point tpollend = std::chrono::high_resolution_clock::now();
					tpollend = MPI_Wtime();
					//tpoll += std::chrono::duration_cast<std::chrono::duration<double>>(tpollend - tpollstart).count();
					tpoll += tpollend - tpollstart;

					char recv_buf[count];

					//std::chrono::high_resolution_clock::time_point trecvstart = std::chrono::high_resolution_clock::now();
					double trecvstart, trecvend;
					trecvstart = MPI_Wtime();
					MPI_Status recv_status;
					MPI_Recv(&recv_buf, count, MPI_CHAR, 1, 1, MPI_COMM_WORLD, &recv_status);
					//std::chrono::high_resolution_clock::time_point trecvend = std::chrono::high_resolution_clock::now();
					trecvend = MPI_Wtime();
					//trecv += std::chrono::duration_cast<std::chrono::duration<double>>(trecvend - trecvstart).count();
					trecv += trecvend - trecvstart;

					//std::cout << "Host 1 received a message from Bluefield 1!\n";

					//std::cout << recv_buf[0] << "\n";
					if (recv_buf[0] != message[0] + 1) {
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
					int count = bfdoffload::Poll(0, 0);
					char recv_buf[count];

					MPI_Status recv_status;
					MPI_Recv(&recv_buf, count, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &recv_status);

					recv_buf[0]++;

					//std::cout << "Host 2 received a message from Bluefield 2!\n";
					MPI_Request req = bfdoffload::iSendBlock(count, recv_buf, 0, 1);
					//std::cout << "Host 2 sent a message back to Bluefield 2!\n";
					MPI_Wait(&req, MPI_STATUS_IGNORE);


					break;
				}	

			MPI_Barrier(MPI_COMM_WORLD);
			}
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
