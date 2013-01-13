#!/bin/bash

#Export path to libmsr
curr=`pwd`

export PATH_LIBMSR=$curr/librapl/libmsr/lib
#export PATH_LIBMSR=$curr/rapl-bmarks/bmarks/librapl/libmsr/lib
echo $PATH_LIBMSR

#cd librapl/libmsr
#mkdir -p lib
#make clean && make -j8 

#cd mpi
#make clean && make -j8 rapl-icc 

cd lulesh-oct30
make clean && make -j8
msub rapl-runs-small.msub



