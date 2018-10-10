#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include "real.h"

#include "parallelSpmv.h"
#define REP 1000

int main(int argc, char *argv[]) 
{
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_SINGLE, &provided);

    const int root=0;
    int worldRank, worldSize;
    MPI_Comm_rank(MPI_COMM_WORLD,&worldRank);
    MPI_Comm_size(MPI_COMM_WORLD,&worldSize);
    
    #include "parallelSpmvData.h"

    // verifing number of input parameters //
   char exists='t';
   char checkSol='f';
    if (worldRank == root) {
        if (argc < 3 ) {
            printf("Use: %s  Matrix_filename InputVector_filename  [SolutionVector_filename]  \n", argv[0]);     
            exists='f';
        } // endif //
        
        FILE *fh;
        // testing if matrix file exists
        if((fh = fopen(argv[1], "rb")  )   == NULL) {
            printf("No matrix file found.\n");
            exists='f';
        } // end if //
        
        // testing if input file exists
        if((fh = fopen(argv[2], "rb")  )   == NULL) {
            printf("No input vector file found.\n");
            exists='f';
        } // end if //

        // testing if output file exists
        if (argc  >3 ) {
            if((fh = fopen(argv[3], "rb")  )   == NULL) {
                printf("No output vector file found.\n");
                exists='f';
            } else {
                checkSol='t';
            } // end if //
        } // end if //
    } // end if //
    MPI_Bcast(&exists,  1,MPI_CHAR,root,MPI_COMM_WORLD);
    if (exists == 'f') {
       if (worldRank == root) printf("Quitting.....\n");
        MPI_Finalize();
        exit(-1);
    } // end if //
    MPI_Bcast(&checkSol,1,MPI_CHAR,root,MPI_COMM_WORLD);

    
    reader(&n_global,&nnz_global, &n, 
           &off_proc_nnz,
           &row_ptr,&col_idx,&val,
           &row_ptr_off,&col_idx_off,&val_off,
           argv[1], root);

    createCommunicator(&nColsOff, &recvCount,&sendCount, &sendColumns, col_idx_off, &off_proc_nnz, &n, &compressedVec);
    
    // ready to start //    

    real *w=NULL;
    real *v=NULL; // <-- input vector to be shared later
    real *v_off=NULL; // <-- input vector to be shared later
    
    w     = (real *) malloc((n)*sizeof(real)); 
    v     = (real *) malloc((n)*sizeof(real));
    v_off = (real *) malloc((nColsOff)*sizeof(real));

    // reading input vector
    vectorReader(v, &n, argv[2]);
    
    int countR=0, countS=0;
    for (int process=0; process<worldSize; ++process) {
        if (recvCount[process] > 0 ) ++countR;
        if (sendCount[process] > 0 ) ++countS;
    } // end for //
    
    if (countS > 0) requestS = (MPI_Request *) malloc( countS*sizeof(MPI_Request));
    if (countS > 0) requestR = (MPI_Request *) malloc( countR*sizeof(MPI_Request));

    // Timing should begin here//
    double elapsed_time;
    MPI_Barrier(MPI_COMM_WORLD);
    elapsed_time = -MPI_Wtime();
    
    for (int t=0; t<REP; ++t) {
        // cleaning solution vector //
        for(int i=0; i<n; ++i) w[i] = 0.0;

        startComunication(v,v_off,compressedVec,recvCount, sendCount, sendColumns, requestS,requestR);

        // solving the on_proc part while comunication is taken place.
        spmv(w,val,v, row_ptr,col_idx,n);
        
        // waitting for the comunication to finish
        MPI_Waitall(countS, requestS,MPI_STATUS_IGNORE);
        MPI_Waitall(countR, requestR,MPI_STATUS_IGNORE);
        
        // now is time to solve the off_proc part
        if (nColsOff > 0) {
            spmv(w,val_off,v_off, row_ptr_off,col_idx_off,n);
        } // end if//
        MPI_Barrier(MPI_COMM_WORLD);
    } // end for //
    // Timing should end here//
    elapsed_time += MPI_Wtime();

    if (worldRank == root) {
        printf("---> Time taken by %d processes: %g seconds\n",worldSize, elapsed_time);
    } // end if //
    
    
    if (checkSol=='t') {
        real *sol=NULL;
        sol     = (real *) malloc((n)*sizeof(real)); 
        // reading input vector
        vectorReader(sol, &n, argv[3]);
        
        int row=0;
        const real tolerance=1.0e-08;
        real error;
        do {
            error =  fabs(sol[row] - w[row]); //  /fabs(sol[row]);
            if ( error > tolerance ) break;
            ++row;
        } while (row < n); // end do-while //
        
        if (row == n) {
            printf("Solution match in rank %d\n",worldRank);
        } else {    
            printf("For Matrix %s, solution does not match at element %d in rank %d   %20.13e   -->  %20.13e  error -> %20.13e, tolerance: %20.13e \n", 
            argv[1], (row+1),worldRank, sol[row], w[row], error , tolerance  );
        } // end if //
        free(sol);    
    } // end if //


    free(w);
    free(v);
    
    #include "parallelSpmvCleanData.h" 
    MPI_Finalize();
    return 0;    
} // end main() //


/*    

    printf("rank: (%d) ---> ", worldRank);
    for (int j=0; j<n; ++j) {
        printf("%f  ", w[j] );
    } //
    printf("\n");



    printf("rank: (%d) ---> ", worldRank);
    for (int process=0; process<worldSize; ++process) {
        for (int j=0; j<sendCount[process]; ++j) {
            printf("%4d", sendColumns[process][j] );
        } //
        printf("|\t\t");
    } //
    printf("\n");
    MPI_Finalize();
    exit(0);
*/    

