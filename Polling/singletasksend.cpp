#include <mpi.h>
#include <iostream>
//#include <chrono>
#include <unistd.h>
#include <algorithm>

#include "bfd_offload.h"

//Run like so: mpirun -n 4 -H b101,b102,b103,b104 ./singletaskhost | tee ./data/testdata.txt

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


	//##################CONSTANTS##############################
	const int tasks_bundled = 1;
	const int message_len = 1024*tasks_bundled;
	const int repeats = 10;

	const int num_cores = 16;
	const int comm_cores = 2;

    const double time_for_task = (0.001*tasks_bundled) / (num_cores - comm_cores);
    const int num_tasks = 128/tasks_bundled;
    const int offload_step = 16/tasks_bundled;
	//########################################################

	double addedtime = 0;


	//Make sure all vars are initialised
	MPI_Barrier(MPI_COMM_WORLD);
	if (my_rank == 0) {
		std::cout << "No. Tasks: " << num_tasks*tasks_bundled << "   Task Size (no. ints) : " << message_len/16 << "   Time Per Task: " \
		<< time_for_task/tasks_bundled << "   Tasks per Enclave: " << tasks_bundled << "\n";
	}
	for (int offloaded_tasks=0; offloaded_tasks<=num_tasks; offloaded_tasks+=offload_step) {
		double toverall = 0;
		double commstime = 0;
		for (int i=0; i<repeats; i++) {
			switch(my_rank) {
				//First host to send message to its own bluefield (rank 2)
				case 0:
				{
					//Initialise with random numbers
					int message[message_len];
					for (int j = 0; j < message_len; j++) {
						message[j] = (rand() % 10000);
					}
					//std::cout << "The first number is: " << message[0] << "\n";

					//std::chrono::high_resolution_clock::time_point tstart = std::chrono::high_resolution_clock::now();
					double tstart = MPI_Wtime();
                    
                    //Send all tasks off
                    for (int j = 0; j < offloaded_tasks; j++) {
                        //Send message to bfd, rank 2
                        MPI_Request req = bfdoffload::iSendBlock(message_len, message, 2, 0);

                        //std::cout << "Host 1 sent a message!\n";
                        MPI_Wait(&req, MPI_STATUS_IGNORE);
					}

                    //Compute local tasks
                    for (int j = 0; j < num_tasks - offloaded_tasks; j++) {
						usleep(time_for_task * 1e6);
						//toverall += time_for_task;
					}
                    
                    
                    int alloffloadedtasks[offloaded_tasks][message_len];
					//We assume each task to be the same size so constant
					int count = 0;

                    //Recv all results back
                    for (int j = 0; j < offloaded_tasks; j++) {
                        //Receive message from bfd 1, rank 2
                        count = bfdoffload::Poll(2, 5);

                        int recv_buf[message_len];

                        MPI_Status recv_status;
                        MPI_Recv(recv_buf, count, MPI_INT, 2, 5, MPI_COMM_WORLD, &recv_status);
						std::copy(std::begin(recv_buf), std::end(recv_buf), std::begin(alloffloadedtasks[j]));
					}

					//std::cout << "Host 1 received a message from Bluefield 1!\n";

					if (alloffloadedtasks[0][0] != message[0] + 1) {
						if (offloaded_tasks != 0)
						{
							validrun = false;
							goto endloop;
						}
					}

					//std::chrono::high_resolution_clock::time_point tend = std::chrono::high_resolution_clock::now();
					double tend = MPI_Wtime();
					toverall += tend - tstart; //std::chrono::duration_cast<std::chrono::duration<double>>(tend - tstart).count();
					break;
				}


				case 1:
				{
					int alloffloadedtasks[offloaded_tasks][message_len];
					//We assume each task to be the same size so constant
					int count = 0;
					//Poll for message from its own bluefield (this rank can be seen as an idle rank)
					for (int j = 0; j < offloaded_tasks; j++) {
						count = bfdoffload::Poll(3, 2);
						int recv_buf[message_len];
						MPI_Status recv_status;
						MPI_Recv(&recv_buf, count, MPI_INT, 3, 2, MPI_COMM_WORLD, &recv_status);
						std::copy(std::begin(recv_buf), std::end(recv_buf), std::begin(alloffloadedtasks[j]));
					}

					//Calc offloaded tasks
                    for (int j = 0; j < offloaded_tasks; j++) {
						usleep(time_for_task * 1e6);
						//toverall += time_for_task * 1e6;
					}

					//Increment recv_buf for validation
					alloffloadedtasks[0][0]++;

					//std::cout << "Now it is: " << alloffloadedtasks[0][0] << "\n";


					for (int j = 0; j < offloaded_tasks; j++) {
						MPI_Request req = bfdoffload::iSendBlock(count, alloffloadedtasks[j], 3, 3);
						//std::cout << "Host 2 sent a message back to Bluefield 2!\n";
						MPI_Wait(&req, MPI_STATUS_IGNORE);
					}


					break;
				}	


				case 2:
				{
					int alloffloadedtasks[offloaded_tasks][message_len];
					//We assume each task to be the same size so constant
					int count = 0;

					//Poll for message from host
					for (int j = 0; j < offloaded_tasks; j++) {
						count = bfdoffload::Poll(0, 0);
						int recv_buf[message_len];

						MPI_Status recv_status;
						MPI_Recv(&recv_buf, count, MPI_INT, 0, 0, MPI_COMM_WORLD, &recv_status);
						std::copy(std::begin(recv_buf), std::end(recv_buf), std::begin(alloffloadedtasks[j]));
						//std::cout << "I received: " << recv_buf[0] << "\n";
					}
					//std::cout << "Now it is: " << alloffloadedtasks[0][0] << "\n";

					//std::cout << "Bluefield 1 received a message from Host 1!\n";
						

					//Pass message onto Bluefield 2 (rank 3)
					for (int j = 0; j < offloaded_tasks; j++) {
						MPI_Request req = bfdoffload::iSendBlock(count, alloffloadedtasks[j], 3, 1);
						MPI_Wait(&req, MPI_STATUS_IGNORE);
					}

					//Receive message from Bluefield 2
					for (int j = 0; j < offloaded_tasks; j++) {
						count = bfdoffload::Poll(3, 4);
						int recv_buf[message_len];

						MPI_Status recv_status2;
						MPI_Recv(&recv_buf, count, MPI_INT, 3, 4, MPI_COMM_WORLD, &recv_status2);
						std::copy(std::begin(recv_buf), std::end(recv_buf), std::begin(alloffloadedtasks[j]));
					}
					//std::cout << "Now it is: " << alloffloadedtasks[0][0] << "\n";

					//std::cout << "Bluefield 1 received a message from Bluefield 2!\n";

					//Pass message back to Host 1
					for (int j = 0; j < offloaded_tasks; j++) {
						MPI_Request req2 = bfdoffload::iSendBlock(count, alloffloadedtasks[j], 0, 5);
						MPI_Wait(&req2, MPI_STATUS_IGNORE);
					}


					break;
				}


				case 3:
				{
					int alloffloadedtasks[offloaded_tasks][message_len];
					//We assume each task to be the same size so constant
					int count = 0;

					//Poll for message from Bluefield 1
					for (int j = 0; j < offloaded_tasks; j++) {
						count = bfdoffload::Poll(2, 1);
						int recv_buf[message_len];

						MPI_Status recv_status;
						MPI_Recv(&recv_buf, count, MPI_INT, 2, 1, MPI_COMM_WORLD, &recv_status);
						std::copy(std::begin(recv_buf), std::end(recv_buf), std::begin(alloffloadedtasks[j]));
					}

					//std::cout << "Bluefield 2 received a message from Bluefield 1!\n";
					
					//Pass on message to Host 2
					for (int j = 0; j < offloaded_tasks; j++) {
						MPI_Request req = bfdoffload::iSendBlock(count, alloffloadedtasks[j], 1, 2);
						MPI_Wait(&req, MPI_STATUS_IGNORE);
					}
					
					//Poll to receive back from Host 2
					for (int j = 0; j < offloaded_tasks; j++) {
						count = bfdoffload::Poll(1, 3);
						int recv_buf[message_len];

						MPI_Status recv_status2;
						MPI_Recv(&recv_buf, count, MPI_INT, 1, 3, MPI_COMM_WORLD, &recv_status2);
						std::copy(std::begin(recv_buf), std::end(recv_buf), std::begin(alloffloadedtasks[j]));
					}

					//std::cout << "Now it is: " << alloffloadedtasks[0][0] << "\n";
					//std::cout << "Bluefield 2 received a message from Host 2!\n";

					//Pass this message to Bfd 1
					for (int j = 0; j < offloaded_tasks; j++) {
						MPI_Request req2 = bfdoffload::iSendBlock(count, alloffloadedtasks[j], 2, 4);
						MPI_Wait(&req2, MPI_STATUS_IGNORE);
					}


					break;
				}
			}
			MPI_Barrier(MPI_COMM_WORLD);
		}
		if (my_rank == 0) {
			if (validrun == true) {
				//Some time to initialise + do standard code
				if (offloaded_tasks == 0) {
					addedtime = (toverall/repeats) - num_tasks*time_for_task;
				}
				//std::cout << addedtime << "\n";
				//Calculate how long the comms took for this
				commstime = toverall/repeats - (std::max(offloaded_tasks, num_tasks-offloaded_tasks)*time_for_task) - addedtime;
				std::cout << "Offloaded Tasks: " << offloaded_tasks*tasks_bundled << "   Time: " << toverall/repeats << "   Comms Time: " << commstime << "\n";
			}
			else { break; }
		}
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
