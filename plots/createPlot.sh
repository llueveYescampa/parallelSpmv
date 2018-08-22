#!/bin/bash
if [ "$#" -lt 3 ]
then
  echo "Usage: $0  Computer matrixName numberOfProc"
  exit 1
fi


cd $1/$2/gnu


echo Gnu > compiler
paste compiler parallelSpmv.txt  > ../../temp.txt
#paste compiler test_exxon_readerNormal.txt  test_exxon_readerSm.txt test_exxon_readerBSR.txt > ../../temp.txt
rm compiler

cd ../../..

cd $1/$2/intel

echo Intel > compiler
paste compiler parallelSpmv.txt  >> ../../temp.txt
#paste compiler test_exxon_readerNormal.txt  test_exxon_readerSm.txt test_exxon_readerBSR.txt >> ../../temp.txt
rm compiler


cd ../../..

cd $1/$2/pgi

echo Pgi > compiler
paste compiler parallelSpmv.txt  >> ../../temp.txt
#paste compiler test_exxon_readerNormal.txt  test_exxon_readerSm.txt test_exxon_readerBSR.txt >> ../../temp.txt
rm compiler

cd ../../..

gnuplot -c plot.gnp $1 $2 $3

rm `find . -name  temp.txt`

