#pragma once
#include <mpi.h>

class bfdoffload {
    public:
        static MPI_Request iSendBlock(int message_len, int message[], int dest, int tag);

        static int Poll(int rank_from, int tag);

        static int * RecvBlock(int rank_from, int count, int tag);
};