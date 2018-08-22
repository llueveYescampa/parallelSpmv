// this function permits eliminate repeated element of an integer array
// it also creates a new array with the correct size to hold only unique element
// the function retuens the size of the new array.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int cmpfunc (const void * a, const void * b) {
 // this function is to be used with the library qsort()
   return ( *(int*)a - *(int*)b );
} // end of cmpfunc //

int createColIdxMap(int **b,  int *a, const int *n) {
    int *temp = (int *)  malloc( *n * sizeof(int));
    
    memcpy(temp, a, *n*sizeof(int));

    qsort(temp, *n, sizeof(int), cmpfunc);
    
    int count=1;
    for (int i=1; i<*n;++i) {
        if (temp[i-1] != temp[i] ) {
            ++count;
        } // end if //
    } // end for //

   (*b) = (int *)  malloc( count * sizeof(int));  
    
    (*b)[0] = temp[0];
    for (int i=1,j=1; i<*n;++i) {
        if (temp[i-1] != temp[i] ) {
           (*b)[j] = temp[i];
           ++j; 
        } // end if //
    } // end for //
    
    free(temp);

    // compacting the sequence of input vector
    for (int j=0; j<*n; ++j) {
        // running a binary search in the b array
        int l = 0;
        int r = count-1;
        while ( l <= r ) {
            int m = (l+r)/2;
            if (  (*b)[m] < a[j] ) {
                l = m+1;
            } else if (  (*b)[m] > a[j] ) {
                r = m-1;
            } else {
                a[j] = m;
                break;
            } // end if //
        } // end while //
    }  // end for //
    return count;    
} // end of createColIdxMap() //
