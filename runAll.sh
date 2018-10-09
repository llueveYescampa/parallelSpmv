#!/bin/bash
if [ "$#" -ne 1 ]
then
  echo "Usage: $0 matrixName"
  exit 1
fi
matrix=$1

cd buildGnu
make clean
make -j

../runTest.sh parallelSpmv $matrix gnu

cd ../

source setIcc intel64
source setImpi

cd buildIntel
make clean
make -j

../runTest.sh parallelSpmv $matrix intel

cd ../

source setPgi 18.x
source setPgiMpi 18.x

cd buildPgi
make clean
make -j

../runTest.sh parallelSpmv  $matrix pgi

cd ../


