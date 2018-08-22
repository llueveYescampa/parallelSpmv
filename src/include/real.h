#ifndef REAL
    #ifdef DOUBLE
       typedef double real;
       #define MPI_MY_REAL MPI_DOUBLE
    #else
       typedef float real;
       #define MPI_MY_REAL MPI_FLOAT
    #endif
    #define REAL
#endif

