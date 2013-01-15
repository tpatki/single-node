#!/bin/bash

#Export path to libmsr
curr=`pwd`

export PATH_LIBMSR=$curr/librapl-master/libmsr/lib
#export PATH_LIBMSR=$curr/rapl-bmarks/bmarks/librapl/libmsr/lib
echo $PATH_LIBMSR

cd librapl-master/libmsr
mkdir -p lib
make clean && make -j8 

cd mpi
make clean && make -j8 rapl-icc 

#cd ../../../lulesh-oct30
#make clean && make

#cd lulesh
#make clean && make

#cd ../../../sphot
#make clean && make

cd ../../../NAS-MZ
make clean 
mkdir -p bin
sh makeBT_MZ.sh
sh makeSP_MZ.sh
sh makeLU_MZ.sh

#cd ../synthetic/scale-cpu
#make clean && make

#cd ../no-scale-cpu-comm
#make clean && make

#cd ../no-scale-mem-comm
#make clean && make

#cd ../scale-mem
#make clean && make
