#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#include "real.h"
void vectorReader( real *v, const int *n, const char *vectorFile)
{
    int worldRank, worldSize;
    MPI_Comm_rank(MPI_COMM_WORLD,&worldRank);
    MPI_Comm_size(MPI_COMM_WORLD,&worldSize);
    MPI_Status status;
    int acumulate=0;
    
    if (worldSize > 1) {
        if (worldRank == 0) {
            acumulate+=*n;
            MPI_Send(&acumulate,1,MPI_INT, (worldRank+1),100, MPI_COMM_WORLD);
            acumulate-=*n;
        } else  if ( worldRank == worldSize-1) {
            MPI_Recv(&acumulate,1,MPI_INT, (worldRank-1),100, MPI_COMM_WORLD,&status);
        } else {
            MPI_Recv(&acumulate,1,MPI_INT, (worldRank-1),100, MPI_COMM_WORLD, &status);
            acumulate+=*n;
            MPI_Send(&acumulate,1,MPI_INT, (worldRank+1),100, MPI_COMM_WORLD);
            acumulate-=*n;
        } // end if //
    } // end if //

    const size_t offset = (acumulate )* sizeof(real) ;

    // opening vector file to read values
    FILE *filePtr;
    filePtr = fopen(vectorFile, "rb");
    // reading cols vector (n) values //
    fseek(filePtr, offset, SEEK_SET);
    if ( !fread(v, sizeof(real), (size_t) *n, filePtr)) exit(0);
    fclose(filePtr);
    // end of opening vector file to read values
} // end of vectoReader //
