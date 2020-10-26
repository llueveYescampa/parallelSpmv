#include <mpi.h>
#define LOW(id,p,n)  ((id)*(n)/(p))
#define HIGH(id,p,n) (LOW((id)+1,p,n)-1)
#define SIZE(id,p,n) (LOW((id)+1,p,n)-LOW(id,p,n)) // eblack


void getRowsNnzPerProc(int *rowsPP, int *nnzPP, const int *global_n, const int *global_nnz,  const int *row_Ptr) 
{
    int worldSize;
    MPI_Comm_size(MPI_COMM_WORLD,&worldSize);
    
    int lowRow=0, upRow;
    int reducedWorldSize= worldSize;
    int reducedNnz=*global_nnz;
    int nnzLimit = row_Ptr[lowRow] + SIZE(0,reducedWorldSize, reducedNnz);
    int partition=0;    

    for (int row = 0; row<*global_n; ++row) {
        if ( row_Ptr[row+1] >=  nnzLimit ) { 
            if ( ( row_Ptr[row+1] - nnzLimit)  <=  nnzLimit - row_Ptr[row]   ) {
                upRow = row;
            } else {
                upRow = row-1;
            } // end if //
            rowsPP[partition] = upRow-lowRow+1;
            nnzPP[partition]  = row_Ptr[upRow+1] - row_Ptr[lowRow];
            reducedNnz -=  (row_Ptr[upRow+1]-row_Ptr[lowRow]);
            --reducedWorldSize;
            lowRow=upRow+1;
            if (partition < worldSize-1 ) nnzLimit= row_Ptr[lowRow] + SIZE(0,reducedWorldSize, reducedNnz);
            ++partition;
        } // end if // 
    } // end for //

/*
    double nnzIncre = (double ) *global_nnz/ (double) worldSize;
    double lookingFor=nnzIncre;
    int startRow=0, endRow;
    int partition=0;

    for (int row=0; row<*global_n; ++row) {    
        if ( (double) row_Ptr[row+1] >=  lookingFor ) { 
            // search for smallest difference
            if ( row_Ptr[row+1] - lookingFor  <= lookingFor - row_Ptr[row]  ) {
                endRow = row;
            } else {
                endRow = row-1;
            } // end if //
            
            rowsPP[partition] = endRow-startRow+1;
            nnzPP[partition]  = row_Ptr[endRow+1] - row_Ptr[startRow];
             
            startRow = endRow+1;
            ++partition;
            if (partition < worldSize-1) {
               lookingFor += nnzIncre;
            } else {
                lookingFor=*global_nnz;
            } // end if //   
        } // end if // 
    } // end for //
*/    
} // end of getRowsPerProc //
