#!/bin/bash

#Export path to libmsr
curr=`pwd`

export PATH_LIBMSR=$curr/librapl-master/libmsr/lib
#export PATH_LIBMSR=$curr/rapl-bmarks/bmarks/librapl/libmsr/lib
echo $PATH_LIBMSR

#cd librapl-master/libmsr
#mkdir -p lib
#make clean && make -j8

#cd mpi
#make clean && make -j8 rapl-icc

cd NAS-MZ/
#make clean
#mkdir -p bin
sh makeSP_MZ.sh

cd SP-MZ/power
msub rapl-runs-small.msub
msub rapl-runs-small.msub
msub rapl-runs-small.msub

