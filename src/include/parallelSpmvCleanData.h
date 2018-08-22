    free(recvCount);
    free(sendCount);

    for (int process=0; process<worldSize; ++process){
        free(compressedVec[process]);
        free(sendColumns[process]);
    } // end for /
    free(compressedVec);
    free(sendColumns);

    free(requestS);
    free(requestR);
    
    free(row_ptr);
    free(col_idx);
    free(val);
    free(row_ptr_off);
    free(col_idx_off);
    free(val_off);

    
