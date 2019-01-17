#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "real.h"

#include "parallelSpmv.h"

int createColIdxMap(int **b,  int *a, const int *n);

void createCommunicator(int *nColsOff,
                        int **recvCount,
                        int **sendCount,
                        int ***sendColumns,
                        int *col_idx_off,
                        const int *off_node_nnz,                     
                        const int *nRows,
                        real ***compressedVec
                        )                        
{
    int worldRank, worldSize;
    MPI_Comm_rank(MPI_COMM_WORLD,&worldRank);
    MPI_Comm_size(MPI_COMM_WORLD,&worldSize);

    int *off_proc_column_map=NULL;
    if (*off_node_nnz)  {
        *nColsOff=  createColIdxMap(&off_proc_column_map,col_idx_off,off_node_nnz ); 
    } // end if //


    // creating the firstColumn array
    int *firstColumn;
    firstColumn     = (int *) malloc( (worldSize+1) * sizeof(int)); 
    firstColumn[0]=0;
    
    MPI_Allgather(nRows, 1,MPI_INT,&firstColumn[1], 1,MPI_INT,MPI_COMM_WORLD);

    for (int i=1; i<=worldSize; ++i) {
        firstColumn[i] += firstColumn[i-1];
    } // enf for //
    // end of creating the firstColumn array


    (*recvCount) = (int *) calloc( (unsigned int) worldSize,sizeof(int)); 
    // the (unsigned int) is to avoid a wird warning message
    (*sendCount) = (int *) calloc(worldSize,sizeof(int));
    
    // finding rank holding each off_proc column
    // and establishing the receive count arrays
    // process: is the rank
    // recvCount[process]: how many to receive from that rank 
    for (int i=0; i<*nColsOff; ++i) {
        int process=0;
        while( off_proc_column_map[i] < firstColumn[process] ||  
               off_proc_column_map[i] >= firstColumn[process+1]  ) {
            ++process;
        }// end while //
        ++(*recvCount)[process];
    } // end for


    // request arrays for communication  - two per process (send/recv)
    MPI_Request *requestS=NULL,*requestR=NULL;
    requestS = (MPI_Request *) malloc(worldSize*sizeof(MPI_Request));
    requestR = (MPI_Request *) malloc(worldSize*sizeof(MPI_Request));
    
    // establishing the send count arrays from the receive count arrays
    // j: is the rank
    // sendCount[j]: how many to send to rank j
    for (int process=0; process <worldSize; ++process ) {
        MPI_Isend((*recvCount+process),1,MPI_INT,process, 123,MPI_COMM_WORLD,&requestS[process]);
        MPI_Irecv((*sendCount+process),1,MPI_INT,process, 123,MPI_COMM_WORLD,&requestR[process]);
    } // end for //

    // Crerating a 2d-array capable to hold rows of
    // independent size to store the lists of columns 
    // this rank need to receive.
    int **reciveColumns; 
    reciveColumns = (int **) malloc(worldSize*sizeof(int *));
    for (int process=0; process<worldSize; ++process){
        reciveColumns[process] = (int *) malloc((*recvCount)[process]*sizeof(int ));
    } // end for //
    
    // filling the reciveColumns arrays
    for (int process=0, k=0; process<worldSize; ++process){
        for (int i=0; i<(*recvCount)[process]; ++i, ++k){
            reciveColumns[process][i] = off_proc_column_map[k];
        } // end for //
    } // end for //
    
    MPI_Waitall(worldSize,requestR,MPI_STATUS_IGNORE);
    MPI_Waitall(worldSize,requestS,MPI_STATUS_IGNORE);


    // Crerating a 2d-array capable to hold rows of
    // independent size to store the lists of columns 
    // this rank need to send.
    (*sendColumns)   = (int **) malloc(worldSize*sizeof(int *));
    
    for (int process=0; process<worldSize; ++process) {
        (*sendColumns)[process] = (int *) malloc((*sendCount)[process]*sizeof(int ));
    } // end for //

    // end of establishing the send/receive count arrays


    
    // Communicating the receive lists to sending processes to create the send lists
    for (int process=0; process <worldSize; ++process) {
        if ((*recvCount)[process] > 0) {
            MPI_Isend(reciveColumns[process],(*recvCount)[process],MPI_INT,process, 321,MPI_COMM_WORLD,&requestS[process] );
        } // end if //   
        
        if ((*sendCount)[process] > 0) {
            MPI_Irecv(  (*sendColumns)[process],(*sendCount)[process],MPI_INT,process, 321,MPI_COMM_WORLD,&requestR[process] );
        } // end if //   
    } // end for //
    

    MPI_Waitall(worldSize,requestR,MPI_STATUS_IGNORE);
    MPI_Waitall(worldSize,requestS,MPI_STATUS_IGNORE);
    // end of Communicating the receive lists to sending processes to create the send lists
    free(requestS);
    free(requestR);
    
    
    // making sendColumns relative to this process
    for (int process=0; process<worldSize; ++process) {
        for (int j=0; j< (*sendCount)[process]; ++j) {
            (*sendColumns)[process][j] -= firstColumn[worldRank];
        } // end for //
    } // end for //
    
    
    for (int process=0; process<worldSize; ++process){
        free(reciveColumns[process]);
    } // end for /
    
    free(reciveColumns);
    free(firstColumn);

    // finaly, compressedVec is an 1D-array holding values on x_ptr to send to 
    // other process to build the x_off_ptr. The compressedVec could be as large
    // as the number of columns(rows) of this rank. It is used during the solution
    // to in the Isend/Irecv phase of the Spmv solution.

    (*compressedVec) = (real **) malloc(worldSize*sizeof(real *));
    
    for (int process=0; process<worldSize; ++process) {
        (*compressedVec)[process] = (real *) malloc((*sendCount)[process]*sizeof(real ));
    } // end for //

} // end of createCommunicator() //
