#!/bin/bash
#MSUB -lpartition=cab
#MSUB -A lc
#MSUB -l nodes=32
#MSUB -q pbatch
#MSUB -l walltime=04:00:00

set -x

i=4
minthreads=4

j=4
maxnodes=4

export PKG_MINW_MINS=0x38198
export PP0_MINW_MINS=0x38198
export DRAM_MINW_MINS=0x38078
export MSR_PKG_POWER_LIMIT=0x0
export MSR_PP0_POWER_LIMIT=0x0
export MSR_DRAM_POWER_LIMIT=0x0
export MSR_RANK_MOD=1 #One  task per node, the rest are threads
export SAMPLE=1

t=0

for t in 0 1
do

export TURBO_ENABLE=$t

#unbounded measurement
export READ_WRITE=0
export READ_ONLY=1

i=4
j=4
        while [ $j -le $maxnodes ]
        do
        i=4
        while [ $i -ge $minthreads ]
        do
#                export OMP_NUM_THREADS=$i
                export MSR_RANK_MOD=$i
                name=`date +%Y_%m_%d_%H_%M_%S_%N`
                mkdir $name
                cd $name
#                touch out.dat
                env > env
                echo $name >> info
                uname -a >> info
          #      srun --nodes=1 --ntasks=1 -ppbatch cat /proc/cpuinfo | grep MHz >> info
          #      srun --nodes=1 --ntasks=1 -o host_info hostname
                echo 'tasks/node: '$i >> info
                echo 'nodes: ' $j >> info

             #  srun --nodes=$j --ntasks=$(($j * $i)) -o out.dat -e f.err ../mpi-spin 2
               mpirun -np=$(($j * $i)) ../mpi-spin 1


                cd ../

                i=$(($i-2))
        done
        j=$(($j*2))
        done




#51, 65, 80 and 95W. Smallest time window.

export READ_WRITE=1
export READ_ONLY=0

for pkg in 0x38198 0x038208 0x038280 0x0382f8
do
i=4
j=4
export MSR_PKG_POWER_LIMIT=${pkg}
	while [ $j -le $maxnodes ]
	do
	i=4
	while [ $i -ge $minthreads ]
	do
		#export OMP_NUM_THREADS=$i
		export MSR_RANK_MOD=1
		name=`date +%Y_%m_%d_%H_%M_%S_%N`
		mkdir $name
		cd $name
		touch out.dat
		env > env
		echo $name >> info
		uname -a >> info
		echo 'tasks/node: '$i >> info
		echo 'nodes: ' $j >> info

		mpirun -np=$(($j * $i)) ../mpi-spin 2 

		cd ../

		i=$(($i-2))
	done
	j=$(($j*2))
	done
done
done

#
