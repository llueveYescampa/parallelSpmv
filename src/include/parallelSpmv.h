void reader(int *gn, int *gnnz, int *n,  int *off_proc_nnz, 
            int **rPtr,int **cIdx,real **v,int **rPtrO,int **cIdxO,real **vO,
            const char *matrixFile, const int root);

void vectorReader(real *v, const int *n, const char *vectorFile);
                        
void createCommunicator( int *nColsOff,
                         int **recvCount,
                         int **sendCount,
                         int ***sendColumns,
                         int *col_idx_off,
                         const int *off_node_nnz,
                         const int *n,
                         real ***compressedVec
                         );                        
                         

void startComunication(real *v,
                       real *v_off, 
                       real **compressedVec,
                       const int *recvCount,
                       const int *sendCount, 
                       int **sendColumns, 
                       MPI_Request *requestS,
                       MPI_Request *requestR
                        );

void spmv(real *b, real *__restrict__ val, real *x, int *row_ptr, int *col_idx, int nRows);                         
