#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "real.h"

void startComunication(real *x_ptr,
                       real *x_off_ptr,
                       real **compressedVec,
                       const int *recvCount,
                       const int *sendCount,  
                       int **sendColumns,
                       MPI_Request *requestS,
                       MPI_Request *requestR
                        )

{
    int worldRank, worldSize;
    MPI_Comm_rank(MPI_COMM_WORLD,&worldRank);
    MPI_Comm_size(MPI_COMM_WORLD,&worldSize);
    
    for (int process=0,indx=0, s=0; process < worldSize; indx += recvCount[process++] ) {
        // need to compress data to send inside compressedVec
        if (sendCount[process] > 0) {                   // need to send to process j
            for (int i=0;  i<sendCount[process]; ++i) {
                    compressedVec[process][i] = x_ptr[sendColumns[process][i]];
            } // end for //
            MPI_Isend(compressedVec[process],sendCount[process],MPI_MY_REAL,process, 231,MPI_COMM_WORLD,&requestS[s++] );
        } // end if //
    } // end for //
    
    for (int process=0,indx=0, r=0; process < worldSize; indx += recvCount[process++] ) {
        // forming x_off_ptr array with contributions from each rank
        if (recvCount[process] > 0) {
            MPI_Irecv(&x_off_ptr[indx],recvCount[process],MPI_MY_REAL,process, 231,MPI_COMM_WORLD,&requestR[r++] );
        } // end if //    
    } // end for //

} // end of startComunication() //
