#!/bin/bash
if [ "$#" -ne 3 ]
then
  echo "Usage: $0 parallelSpmv matrixName compiler"
  exit 1
fi

tempFilename=$(hostname)'_anyTempFileNameWillWork.txt'
outputFilename=$1_cluster.txt

nloops=5

# Determining MPI implementation and binding options #
MPI=`mpiexec --version | head -1 | awk '{print $1}' `

if [ "$MPI" == "HYDRA" ]; then
    echo "MPICH"
    bindings="--bind-to socket"
    export HYDRA_TOPO_DEBUG=1
    #export MPIR_CVAR_CH3_PORT_RANGE=10000:10100
elif [ "$MPI" == "Intel(R)" ]; then
    echo "Intel MPI"
    bindings="-genv I_MPI_PIN_DOMAIN=socket  -genv I_MPI_PIN_ORDER=scatter  -genv I_MPI_DEBUG=4  -genv I_MPI_FABRICS=shm:ofi"
elif [ "$MPI" == "mpiexec" ]; then
    echo "open-mpi"
    bindings="--bind-to socket --map-by socket  --report-bindings "
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

for j in  `seq 1 $nloops`; do
    echo run number: $j
    echo  $1  ../matrices/$2".mm_bin" ../matrices/$2".in_bin" 
    if [ "$MPI" == "HYDRA" ]; then
        mpiexec $bindings  -hosts blackPanther,blackEngineering -n 4 -ppn 2 $1  ../matrices/$2".mm_bin" ../matrices/$2".in_bin"   ../matrices/$2".out_bin"  | grep taken >>  $tempFilename
    elif [ "$MPI" == "Intel(R)" ]; then
        mpiexec $bindings  -hosts blackPanther,blackEngineering -n 4 -ppn 2 $1  ../matrices/$2".mm_bin" ../matrices/$2".in_bin"   ../matrices/$2".out_bin"  | grep taken >>  $tempFilename    
    elif [ "$MPI" == "mpiexec" ]; then
        mpiexec $bindings  -host blackPanther:2,blackEngineering:2 -n 4  $1 ../matrices/$2".mm_bin" ../matrices/$2".in_bin"   ../matrices/$2".out_bin"  | grep taken >>  $tempFilename        
    fi
    
done


mkdir -p ../plots/$(hostname)/$2/$3

#cat $tempFilename | awk 'BEGIN{}   { printf("%d %f\n", $5,$7)}  END{}' |  sort  -k1,1n -k2,2n |  awk 'BEGIN{ prev=-1} { if ($1 != prev) { print $0; prev=$1}  } END{}' > ../plots/$(hostname)/$2/$3/$outputFilename
cat $tempFilename | awk 'BEGIN{}   { printf("%d %f\n", $5,$7)}  END{}' |  sort  -k1,1n -k2,2n |   awk 'BEGIN{ prev=-1} { if ($1 != prev) { printf("%f  ",$2); prev=$1}  } END{printf("\n")}'  > ../plots/$(hostname)/$2/$3/$outputFilename

rm $tempFilename

