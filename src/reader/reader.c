#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "real.h"

void getRowsNnzPerProc(int *rowsPP,int *nnzPP,  const int *global_n, const int *global_nnz, const int *row_Ptr);

void reader( int *n_global, 
             int *nnz_global, 
             int *n,
             int *off_proc_nnz, 
             int **rowPtr, int **colIdx, real **val,
             int **rowPtrO, int **colIdxO, real **valO,
             const char *matrixFile, 
             const int root)
{
    
    int worldRank, worldSize;
    MPI_Comm_rank(MPI_COMM_WORLD,&worldRank);
    MPI_Comm_size(MPI_COMM_WORLD,&worldSize);
    
    FILE *filePtr;

    int *rowsPP,*nnzPP;
    
    rowsPP    = (int *) malloc(  worldSize    * sizeof(int)); 
    nnzPP     = (int *) malloc(  worldSize    * sizeof(int)); 
    
    
    int *firstColumn; 
    firstColumn = (int *) malloc( (worldSize+1) * sizeof(int)); 
    firstColumn[0] = 0;
    int fColumn, lColumn;

    if (worldRank == root) {
        filePtr = fopen(matrixFile, "rb");
        
        // reading global nun rows //
        if ( !fread(n_global, sizeof(int), 1, filePtr) ) exit(0); 

        // reading global nnz //
        if ( !fread(nnz_global, sizeof(int), (size_t) 1, filePtr)) exit(0);

        int *rows_Ptr;
        rows_Ptr = (int *) malloc((*n_global+1)*sizeof(int));    
        // reading rows vector (n+1) values //
        if ( !fread(rows_Ptr, sizeof(int), (size_t) (*n_global+1), filePtr)) exit(0);
        
        getRowsNnzPerProc(rowsPP, nnzPP,n_global,nnz_global, rows_Ptr);
        
        // forming on-proc columns per proc
        for (int i=0; i<worldSize; ++i) {
            firstColumn[i+1] = firstColumn[i] + rowsPP[i];
        } // end for //
        free(rows_Ptr);
        fclose(filePtr);
    } // end if //
    
    MPI_Scatter(&firstColumn[worldRank]  ,1,MPI_INT, &fColumn ,1,MPI_INT,root,MPI_COMM_WORLD);
    MPI_Scatter(&firstColumn[worldRank+1],1,MPI_INT, &lColumn ,1,MPI_INT,root,MPI_COMM_WORLD);
    --lColumn;
    free(firstColumn);

    
    MPI_Bcast(n_global,1,MPI_INT,root,MPI_COMM_WORLD);
    MPI_Bcast(nnz_global,1,MPI_INT,root,MPI_COMM_WORLD);
    //MPI_Bcast(firstColumn,(worldSize+1),MPI_INT,root,MPI_COMM_WORLD);

    // obtaining number of rows per processor
    MPI_Scatter(rowsPP,1,MPI_INT,n    ,1,MPI_INT,root,MPI_COMM_WORLD);
    
    // obtaining number of non-zeros per processor
    int nnz;
    MPI_Scatter(nnzPP, 1,MPI_INT,&nnz,1,MPI_INT,root,MPI_COMM_WORLD);
    
    int *offsetColArry, *offsetRowArry;
    offsetRowArry    = (int *) malloc(  worldSize    * sizeof(int)); 
    offsetColArry    = (int *) malloc(  worldSize    * sizeof(int)); 
    
    if (worldRank == root) {
        offsetRowArry[0]=0;
        offsetColArry[0]=0;
        for (int i=1; i<worldSize ; ++i) {
            offsetRowArry[i] = offsetRowArry[i-1] +  rowsPP[i-1];   
            offsetColArry[i] = offsetColArry[i-1] +   nnzPP[i-1];   
        } // end for //
    } // end if //
    
    int offsetR, offsetC;
    MPI_Scatter(offsetRowArry,  1,MPI_INT,&offsetR,  1,MPI_INT,root,MPI_COMM_WORLD);
    MPI_Scatter(offsetColArry,  1,MPI_INT,&offsetC,  1,MPI_INT,root,MPI_COMM_WORLD);

    free (rowsPP);
    free (nnzPP);
    free (offsetRowArry);
    free (offsetColArry);

        
    size_t offset;
    offset=(3 + *n_global + offsetC)*sizeof(int);
    
    // each process read the columns associated with their non-zeros
    int *cols_Ptr;
    cols_Ptr = (int *) malloc(nnz*sizeof(int));
    
    // opening file to read column information for this process
    filePtr = fopen(matrixFile, "rb");
    // reading cols vector (nnz) values //
    fseek(filePtr, offset, SEEK_SET);
    if ( !fread(cols_Ptr, sizeof(int), (size_t) nnz, filePtr)) exit(0);
    // end of opening file to read column information for this process
    
    
    // Going nodal //
    
    
    
    // determining on_proc_nnz and of_proc_nnz for this process
    int on_proc_nnz=0;
    for (int i=0; i<nnz; ++i) {
        if (cols_Ptr[i] >= fColumn  &&  cols_Ptr[i] <= lColumn  ) {
            ++on_proc_nnz;
        } else {
            ++*off_proc_nnz;
        } // end if 
    } // end for //
    // end of determining on_proc_nnz and of_proc_nnz  for this process
    
    // allocating for on-proc solution //
    (*rowPtr) = (int *)  malloc( (*n+1)         * sizeof(int)); 
    (*rowPtr)[0] = 0;
    (*colIdx) = (int *)  malloc( (on_proc_nnz) * sizeof(int)); 
    (*val)    = (real *) malloc( (on_proc_nnz) * sizeof(real)); 
    
    // allocating for off-proc solution if needed //
    if (*off_proc_nnz) {
        (*rowPtrO) = (int *)  malloc( (*n+1)          * sizeof(int)); 
        (*rowPtrO)[0] = 0;
        (*colIdxO) = (int *)  malloc( (*off_proc_nnz) * sizeof(int)); 
        (*valO)    = (real *) malloc( (*off_proc_nnz) * sizeof(real)); 
    } // end if //

    
    // each process read the rows pointers
    offset=(2 + offsetR)*sizeof(int);
    int *rows_Ptr;
    rows_Ptr = (int *) malloc((*n+1)*sizeof(int));


    fseek(filePtr, offset, SEEK_SET);
    if ( !fread(rows_Ptr, sizeof(int), (size_t) (*n+1), filePtr)) exit(0);
    // each process read the rows pointers

    
    // each process will read the vals one by one
    offset=(3 + *n_global + *nnz_global  ) * sizeof(int) + offsetC * sizeof(real);
    fseek(filePtr, offset, SEEK_SET);
    
    for (int i=1,k=0,on=0,off=0; i<= *n; i++) {
        int nnzPR = rows_Ptr[i] - rows_Ptr[i-1];
        int rowCounterOn=0;
        int rowCounterOff=0;
        real temp;
        for (int j=0; j<nnzPR; ++j, ++k ) {
            if ( !fread(&temp, sizeof(real), (size_t) (1), filePtr)) exit(0);
            if (cols_Ptr[k] >=  fColumn  &&  cols_Ptr[k] <=  lColumn  ) {
                // on process data goes here
                ++rowCounterOn;
                (*colIdx)[on] = cols_Ptr[k] - fColumn;
                (*val)[on] = temp;
                ++on;
            } else {
                // off process data goes here
                ++rowCounterOff;
                (*colIdxO)[off] = cols_Ptr[k];
                (*valO)[off] = temp;
                ++off;
            } // end if 
        } // end for //
        (*rowPtr)[i]  = (*rowPtr)[i-1]  + rowCounterOn;
        if (*off_proc_nnz) (*rowPtrO)[i] = (*rowPtrO)[i-1] + rowCounterOff;
    } // end for //
    
    free(rows_Ptr);
    free(cols_Ptr);
    fclose(filePtr);
} // end of reader //
