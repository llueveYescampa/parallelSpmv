#include "real.h"

void spmv(real *__restrict__ b, const real *__restrict__ val, const real *x, const int *row_ptr, const int *col_idx, int nRows)
{
    int start, end;
    for (int i=0; i<nRows; ++i) {
        start = row_ptr[i];
        end = row_ptr[i+1];
        for (int j=start; j<end; ++j) {
            b[i] += val[j] * x[col_idx[j]];
        } // end for //
    } // end for //
} // end of spmv() //

