#!/bin/bash
if [ "$#" -ne 1 ]
then
  echo "Usage: $0 matrixName"
  exit 1
fi

cd buildGnu
make clean
make -j

../runClusterTest.sh parallelSpmv    $1 gnu

cd ../

source setIcc intel64
source setImpi

cd buildIntel
make clean
make -j

../runClusterTest.sh parallelSpmv    $1 intel

#
#cd ../

#source setPgi 18.x
#source setPgiMpi 18.x

#cd buildPgi
#make clean
#make -j

#../runClusterTest.sh parallelSpmv    $1 pgi

#cd ../

