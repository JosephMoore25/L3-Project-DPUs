#include <mpi.h>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <time.h>
#include <chrono>

MPI_Request iSendBlock(int message_len, int message[], int dest){
	MPI_Request request = MPI_REQUEST_NULL;
	MPI_Isend(&message, message_len, MPI_INT, dest, 77, MPI_COMM_WORLD, &request);
	return request;
}

int main(int argc, char **argv) {
	MPI_Init(&argc, &argv);
	//Get num processes in MPI_COMM_WORLD
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	int rank_boundary = world_size / 2;

	int my_rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	int len;
	char name[MPI_MAX_PROCESSOR_NAME];
	MPI_Get_processor_name(name, &len);

	MPI_Status statuses[12];
	MPI_Request request0, request01, request21, request22, request32, request33;	
	
	switch(my_rank) {
		//First host to send message to its own bluefield (rank 2)
		case 0:
		{
			std::chrono::high_resolution_clock::time_point tstart = std::chrono::high_resolution_clock::now();

			//Send message to bfd
			int message_len = 1;
			int message[message_len] = {17};
			request0 = iSendBlock(message_len, message, 2);
			std::cout << "Host 1 sent a message!\n";
			MPI_Wait(&request0, MPI_STATUS_IGNORE);

			//Receive message from bfd 1
			int received_msg7;
                        while (!received_msg7) {
                                MPI_Iprobe(2, 78, MPI_COMM_WORLD, &received_msg7, &statuses[10]);
                                usleep(100);
                        }
                        int count7;
                        MPI_Get_count( &statuses[10], MPI_INT, &count7 );
                        int recv_buf7[count7];

			MPI_Recv(&recv_buf7, count7, MPI_INT, 2, 78, MPI_COMM_WORLD, &statuses[11]);
                        std::cout << "Host 1 received a message from Bluefield 1!\n";

			std::chrono::high_resolution_clock::time_point tend = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> run_time = std::chrono::duration_cast<std::chrono::duration<double>>(tend - tstart);

			std::cout << "Completed full trip in: " << run_time.count() << " seconds\n";
			break;
		}
		case 1:
		{
			//Poll for message from its own bluefield (this rank can be seen as an idle rank)
			int received_msg1;
			while (!received_msg1) {
                                MPI_Iprobe(3, 77, MPI_COMM_WORLD, &received_msg1, &statuses[0]);
                        	usleep(100);
			}
			//usleep(1000000);
			int count;
                        MPI_Get_count( &statuses[0], MPI_INT, &count );
                        int recv_buf[count];
                        //request1 = MPI_REQUEST_NULL;
			
			MPI_Recv(&recv_buf, count, MPI_INT, 3, 77, MPI_COMM_WORLD, &statuses[1]);
			std::cout << "Host 2 received a message from Bluefield 2!\n";

			request01 = iSendBlock(count, recv_buf, 3);
                        //std::cout << "Host 2 sent a message back to Bluefield 2!\n";
                        MPI_Wait(&request01, MPI_STATUS_IGNORE);

			break;
		}	
		case 2:
		{
			//Poll for message from host
			int received_msg2;
			while (!received_msg2) {
                                MPI_Iprobe(0, 77, MPI_COMM_WORLD, &received_msg2, &statuses[2]);
				usleep(100);
                        }
			//usleep(1000000);
			int count;
                        MPI_Get_count( &statuses[2], MPI_INT, &count );
                        int recv_buf[count];

			MPI_Recv(&recv_buf, count, MPI_INT, 0, 77, MPI_COMM_WORLD, &statuses[3]);
			std::cout << "Bluefield 1 received a message from Host 1!\n";
				
			//Pass message onto Bluefield 2 (rank 3)
			request21 = iSendBlock(count, recv_buf, 3);
			MPI_Wait(&request21, MPI_STATUS_IGNORE);

			//Receive message from Bluefield 2
			int received_msg5;
                        while (!received_msg5) {
                                MPI_Iprobe(3, 77, MPI_COMM_WORLD, &received_msg5, &statuses[8]);
                                usleep(100);
                        }
			int count2;
                        MPI_Get_count( &statuses[8], MPI_INT, &count2 );
                        int recv_buf2[count2];

			MPI_Recv(&recv_buf2, count2, MPI_INT, 3, 77, MPI_COMM_WORLD, &statuses[9]);
                        std::cout << "Bluefield 1 received a message from Bluefield 2!\n";

			//Pass message back to Host 1
			request22 = iSendBlock(count2, recv_buf2, 0);
                        MPI_Wait(&request22, MPI_STATUS_IGNORE);

			break;
		}
		case 3:
		{
			//Poll for message from Bluefield 1
			int received_msg3;
			while (!received_msg3) {
                                MPI_Iprobe(2, 77, MPI_COMM_WORLD, &received_msg3, &statuses[4]);
          			usleep(100);
			}
			//usleep(1000000);
			int count;
                        MPI_Get_count( &statuses[4], MPI_INT, &count );
                        int recv_buf[count];
                        
			MPI_Recv(&recv_buf, count, MPI_INT, 2, 77, MPI_COMM_WORLD, &statuses[5]);
			std::cout << "Bluefield 2 received a message from Bluefield 1!\n";
			
			//Pass on message to Host 2
			request32 = iSendBlock(count, recv_buf, 1);
			MPI_Wait(&request32, MPI_STATUS_IGNORE);
			
			//Poll to receive back from Host 2
			int received_msg4;
                        while (!received_msg4) {
                                MPI_Iprobe(1, 77, MPI_COMM_WORLD, &received_msg4, &statuses[6]);
                                usleep(100);
                        }

			int count2;
			MPI_Get_count( &statuses[6], MPI_INT, &count2 );
                        int recv_buf2[count2];
			MPI_Recv(&recv_buf2, count2, MPI_INT, 1, 77, MPI_COMM_WORLD, &statuses[7]);
                        std::cout << "Bluefield 2 received a message from Host 2!\n";

			//Pass this message to Bfd 1
                        request33 = iSendBlock(count2, recv_buf2, 2);
                        MPI_Wait(&request33, MPI_STATUS_IGNORE);

			break;
		}
	}

	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();

	return 0;
}
