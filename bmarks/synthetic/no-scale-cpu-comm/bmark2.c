//Tapasya Patki
//Synthetic - 5
//CPU-bound and not scalable and lots of communication

#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>


//Large is 2^32 atm
#define LARGE 42949672960
#define ITERS 20
#define COMM_ITERS 256
//#define MSG_SZ 65536
#define MSG_SZ 262144
//#define MSG_SZ 10485760


int main(int argc, char * argv[]){

	int myrank, numnodes;
	unsigned long mychunk, smallchunk;
	int root = 0;
	int it,k;
	unsigned long i;
	double start, end;

	int t;
	
	double sendcount, recvcount;
	double *sendbuf, *recvbuf;

	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	MPI_Comm_size(MPI_COMM_WORLD, &numnodes);

	sendbuf = (double*) malloc(sizeof(double) * MSG_SZ * numnodes);
	recvbuf = (double*) malloc(sizeof(double) * MSG_SZ * numnodes);

	if(sendbuf == NULL || recvbuf == NULL){
		printf("Could not allocate memory\n");
		exit(1);
	}
		
	
	for(k=0; k<numnodes;k++){
		for(it=0; it<MSG_SZ;it++){
			sendbuf[it] = (k + it + 0.75);
		}
	}
	
	mychunk = (unsigned long) (LARGE/numnodes);

	if(myrank==0){
		printf("\nEach chunk is: %ld", mychunk);
		printf("\nIterations: %d", ITERS);
		printf("\nNodes: %d", numnodes);
	}

	start=MPI_Wtime();
	

	//MPI_Bcast(&mychunk, 1, MPI_UNSIGNED_LONG, root, MPI_COMM_WORLD);

	for(it=0;it<ITERS;it++){
	#pragma omp parallel for private(i)
		for(i=0; i<mychunk; i++);
	//}


	//Send a large MPI_Alltoall so things don't scale. Repeat.
	
	MPI_Alltoall(sendbuf, MSG_SZ, MPI_DOUBLE, recvbuf, MSG_SZ, MPI_DOUBLE, MPI_COMM_WORLD);

	MPI_Alltoall(sendbuf, MSG_SZ, MPI_DOUBLE, recvbuf, MSG_SZ, MPI_DOUBLE, MPI_COMM_WORLD);
	}

	for(it=0;it<COMM_ITERS;it++){

		MPI_Alltoall(sendbuf, MSG_SZ, MPI_DOUBLE, recvbuf, MSG_SZ, MPI_DOUBLE, MPI_COMM_WORLD);
	}

	end=MPI_Wtime();

	printf("\nTime is: %lf secs\n", end-start);

	free(sendbuf);
	free(recvbuf);

	MPI_Finalize();

return 0;
}
