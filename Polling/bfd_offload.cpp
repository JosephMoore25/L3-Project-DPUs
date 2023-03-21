#include "bfd_offload.h"
#include <unistd.h>

//Sends message of len message_len to dest
MPI_Request bfdoffload::iSendBlock(int message_len, int message[], int dest, int tag){
	MPI_Request request = MPI_REQUEST_NULL;
	MPI_Isend(&message, message_len, MPI_INT, dest, tag, MPI_COMM_WORLD, &request);
	return request;
}

//Polls for message from "rank_from", and receives once found
int bfdoffload::Poll(int rank_from, int tag){
	int received_msg = 0;
	MPI_Status probe_status;
    while (received_msg == 0) {
            MPI_Iprobe(rank_from, tag, MPI_COMM_WORLD, &received_msg, &probe_status);
            //std::cout << "Flag is set to " << received_msg << "\n";
            usleep(100);
    }
    //std::cout << "Flag is set to " << received_msg << "\n";
    int count;
    MPI_Get_count( &probe_status, MPI_INT, &count );
    //std::cout << "Found message from " << rank_from << " with tag " << tag << " and count: " << count << "\n";
	return count;
}

int * bfdoffload::RecvBlock(int rank_from, int count, int tag){
    int* recv_buf = new int[count];
	MPI_Status recv_status;
    //std::cout << "Test1, rank_from = " << rank_from << " and tag is: " << tag << "\n";
	MPI_Recv(recv_buf, count, MPI_INT, rank_from, tag, MPI_COMM_WORLD, &recv_status);
    //std::cout << "Test2\n";
	return recv_buf;
}
