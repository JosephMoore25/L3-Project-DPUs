#include <mpi.h>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <time.h>

//Sends message of len message_len to dest
MPI_Request iSendBlock(int message_len, int message[], int dest){
	MPI_Request request = MPI_REQUEST_NULL;
	MPI_Isend(&message, message_len, MPI_INT, dest, 77, MPI_COMM_WORLD, &request);
	return request;
}

//Polls for message from "rank_from", and receives once found
int Poll(int rank_from){
	int received_msg;
	MPI_Status probe_status;
    while (!received_msg) {
            MPI_Iprobe(rank_from, 77, MPI_COMM_WORLD, &received_msg, &probe_status);
            usleep(100);
    }
    int count;
    MPI_Get_count( &probe_status, MPI_INT, &count );
	return count;
}

int * RecvBlock(int rank_from, int count){
    int * recv_buf;
	MPI_Status recv_status;
	MPI_Recv(recv_buf, count, MPI_INT, rank_from, 77, MPI_COMM_WORLD, &recv_status);
	return recv_buf;
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
			MPI_Request req = iSendBlock(message_len, message, 2);
			std::cout << "Host 1 sent a message!\n";
			MPI_Wait(&req, MPI_STATUS_IGNORE);
			break;
		}
		case 1:
		{
			//Poll for message from its own bluefield (this rank can be seen as an idle rank)
			int count = Poll(3);
			int *recv_buf;
			recv_buf = RecvBlock(3, count);
			std::cout << "Host 2 received a message from Bluefield 2!\n";
			break;
		}	
		case 2:
		{
			//Poll for message from host
			int count = Poll(0);
			int *recv_buf;
			recv_buf = RecvBlock(0, count);
			std::cout << "Bluefield 1 received a message from Host 1!\n";
				
			//Pass message onto Bluefield 2 (rank 3)
			MPI_Request req = iSendBlock(count, recv_buf, 3);
			MPI_Wait(&req, MPI_STATUS_IGNORE);
			break;
		}
		case 3:
		{
			//Poll for message from Bluefield 1
			int count = Poll(2);
			int *recv_buf;
			recv_buf = RecvBlock(2, count);
			std::cout << "Bluefield 2 received a message from Bluefield 1!\n";
			
			//Pass on message to Host 2
			MPI_Request req = iSendBlock(count, recv_buf, 1);
			MPI_Wait(&req, MPI_STATUS_IGNORE);
			break;
		}
	}

	MPI_Finalize();

	return 0;
}
