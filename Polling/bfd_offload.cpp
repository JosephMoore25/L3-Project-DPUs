#include "bfd_offload.h"
#include <unistd.h>

//Sends message of len message_len to dest
MPI_Request bfdoffload::iSendBlock(int message_len, char message[], int dest, int tag){
	MPI_Request request = MPI_REQUEST_NULL;
	MPI_Isend(message, message_len, MPI_CHAR, dest, tag, MPI_COMM_WORLD, &request);
	return request;
}

//Polls for message from "rank_from", and receives once found
int bfdoffload::Poll(int rank_from, int tag){
	int received_msg = 0;
	MPI_Status probe_status;
    while (received_msg == 0) {
            MPI_Iprobe(rank_from, tag, MPI_COMM_WORLD, &received_msg, &probe_status);
            usleep(5);
    }
    int count;
    MPI_Get_count( &probe_status, MPI_CHAR, &count );
    //std::cout << "Found message from " << rank_from << " with tag " << tag << " and count: " << count << "\n";
	return count;
}
