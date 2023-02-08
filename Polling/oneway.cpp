#include <mpi.h>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <time.h>

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

	//Init random seed
	srand(time(NULL));
	MPI_Status status1, status11, status2, status22, status3, status33;
	MPI_Request request0, request21, request22, request31, request32;	
	
	switch(my_rank) {
		//First host to send message to its own bluefield (rank 2)
		case 0:
		{
			//Send message to bfd
			int message_len = 1;
			int message[message_len] = {17};
			request0 = MPI_REQUEST_NULL;
			MPI_Isend(&message, message_len, MPI_INT, 2, 77, MPI_COMM_WORLD, &request0);
			std::cout << "Host 1 sent a message!\n";
			break;
			MPI_Wait(&request0, MPI_STATUS_IGNORE);
		}
		case 1:
		{
			//Poll for message from its own bluefield (this rank can be seen as an idle rank)
			int received_msg1;
			while (!received_msg1) {
                                MPI_Iprobe(3, 77, MPI_COMM_WORLD, &received_msg1, &status1);
                        	usleep(10000);
			}
			//usleep(1000000);
			int count;
                        MPI_Get_count( &status1, MPI_INT, &count );
                        int recv_buf[count];
                        //request1 = MPI_REQUEST_NULL;
			
			MPI_Recv(&recv_buf, count, MPI_INT, 3, 77, MPI_COMM_WORLD, &status11);
			std::cout << "Host 2 received a message from Bluefield 2!\n";
			break;
		}	
		case 2:
		{
			//Poll for message from host
			int received_msg2;
			while (!received_msg2) {
                                MPI_Iprobe(0, 77, MPI_COMM_WORLD, &received_msg2, &status2);
				usleep(10000);
                        }
			//usleep(1000000);
			int count;
                        MPI_Get_count( &status2, MPI_INT, &count );
                        int recv_buf[count];
                        request21 = MPI_REQUEST_NULL;

			MPI_Recv(&recv_buf, count, MPI_INT, MPI_ANY_SOURCE, 77, MPI_COMM_WORLD, &status22);
			std::cout << "Bluefield 1 received a message from Host 1!\n";
			
			//Pass message onto Bluefield 2 (rank 3)
			request22 = MPI_REQUEST_NULL;
			MPI_Isend(&recv_buf, count, MPI_INT, 3, 77, MPI_COMM_WORLD, &request22);
			MPI_Wait(&request22, MPI_STATUS_IGNORE);
			break;
		}
		case 3:
		{
			//Poll for message from Bluefield 1
			int received_msg3;
			while (!received_msg3) {
                                MPI_Iprobe(2, 77, MPI_COMM_WORLD, &received_msg3, &status3);
          			usleep(10000);
			}
			//usleep(1000000);
			int count;
                        MPI_Get_count( &status3, MPI_INT, &count );
                        int recv_buf[count];
                        request31 = MPI_REQUEST_NULL;

			MPI_Recv(&recv_buf, count, MPI_INT, 2, 77, MPI_COMM_WORLD, &status33);
			std::cout << "Bluefield 2 received a message from Bluefield 1!\n";
			
			//Pass on message to Host 2
			request32 = MPI_REQUEST_NULL;
			MPI_Isend(&recv_buf, count, MPI_INT, 1, 77, MPI_COMM_WORLD, &request32);
			MPI_Wait(&request32, MPI_STATUS_IGNORE);
			break;
		}
	}

	MPI_Finalize();

	return 0;
}
