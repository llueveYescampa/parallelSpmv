#!/bin/bash
if [ "$#" -ne 3 ]
then
  echo "Usage: $0 parallelSpmv matrixName compiler"
  exit 1
fi

tempFilename=$(hostname)'_anyTempFileNameWillWork.txt'
outputFilename=$1.txt

nloops=5

# Determining MPI implementation and binding options #
MPI=`mpiexec --version | head -1 | awk '{print $1}' `

if [ "$MPI" == "HYDRA" ]; then
    echo "MPICH"
    bindings="--bind-to socket"
    export HYDRA_TOPO_DEBUG=1
elif [ "$MPI" == "Intel(R)" ]; then
    echo "Intel MPI"
    bindings="-genv I_MPI_PIN_DOMAIN=core -genv I_MPI_PIN_ORDER=spread -genv I_MPI_DEBUG=4"
elif [ "$MPI" == "mpiexec" ]; then
    echo "open-mpi"
    bindings="--bind-to core --report-bindings"
fi
# end of Determining MPI implementation and binding options #

npt=`grep -c ^processor /proc/cpuinfo`
numaNodes=`lscpu | grep "NUMA node(s):" | awk '{}{print $3}{}'`
tpc=`lscpu | grep "Thread(s) per core:" | awk '{}{print $4}{}'`
np="$(($npt / $tpc))"
npps="$(($np / $numaNodes))"
npm1="$(($np - 1))"


if [ -n "$PGI" ]; then
    echo "Pgi Compiler"
elif [ -n "$INTEL_LICENSE_FILE" ]; then
    echo "Intel Compiler"
else
    echo "Gnu Compiler"
fi

rm -f $tempFilename
for i in  `seq 1 $np`; do
    for j in  `seq 1 $nloops`; do
        echo run number: $j, using $i processes 
        echo  $1  ../matrices/$2".mm_bin" ../matrices/$2".in_bin" 
        mpiexec $bindings  -n $i  $1  ../matrices/$2".mm_bin" ../matrices/$2".in_bin"   ../matrices/$2".out_bin"  | grep taken >>  $tempFilename
    done
done

mkdir -p ../plots/$(hostname)/$2/$3

#cat $tempFilename | awk 'BEGIN{}   { printf("%d %f\n", $5,$7)}  END{}' |  sort  -k1,1n -k2,2n |  awk 'BEGIN{ prev=-1} { if ($1 != prev) { print $0; prev=$1}  } END{}' > ../plots/$(hostname)/$2/$3/$outputFilename
cat $tempFilename | awk 'BEGIN{}   { printf("%d %f\n", $5,$7)}  END{}' |  sort  -k1,1n -k2,2n |   awk 'BEGIN{ prev=-1} { if ($1 != prev) { printf("%f  ",$2); prev=$1}  } END{printf("\n")}'  > ../plots/$(hostname)/$2/$3/$outputFilename

rm $tempFilename

