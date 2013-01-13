#!/bin/bash

#mkdir bin

make bt-mz CLASS=A NPROCS=16

make -j8 bt-mz CLASS=C NPROCS=1
make -j8 bt-mz CLASS=C NPROCS=2
make -j8 bt-mz CLASS=C NPROCS=4
make -j8 bt-mz CLASS=C NPROCS=8
make -j8 bt-mz CLASS=C NPROCS=10
make -j8 bt-mz CLASS=C NPROCS=12
make -j8 bt-mz CLASS=C NPROCS=14
make -j8 bt-mz CLASS=C NPROCS=16
make -j8 bt-mz CLASS=C NPROCS=18
make -j8 bt-mz CLASS=C NPROCS=20
make -j8 bt-mz CLASS=C NPROCS=22
make -j8 bt-mz CLASS=C NPROCS=24
make -j8 bt-mz CLASS=C NPROCS=26
make -j8 bt-mz CLASS=C NPROCS=28
make -j8 bt-mz CLASS=C NPROCS=30
make -j8 bt-mz CLASS=C NPROCS=232
#make -j8 bt-mz CLASS=C NPROCS=64
#make -j8 bt-mz CLASS=C NPROCS=128
#make bt-mz CLASS=C NPROCS=256
#make bt-mz CLASS=C NPROCS=512
#make bt-mz CLASS=C NPROCS=1024