//Tapasya Patki
//Synthetic - 4
//Memory-bound and not-scalable and lots of communication

#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

//Large is 2^32 atm
#define LARGE 4096
#define ITERS 2
//Need to run same number of iters as the no-scale-cpu version; which is 20.
#define COMM_ITERS 20 
#define COMM2_ITERS 256
//Total array size = 65536 * 8 bytes; 512 MB
//#define FIXED_SZ 131072
//#define FIXED_SZ 16777216
#define FIXED_SZ (20 * 1024 * 1024 * 16)
#define MSG_SZ 262144


int main(int argc, char * argv[]){

	int myrank, numnodes;
	unsigned long mychunk;
	int it,k;
	unsigned long i,j;
	double start, end;
	int t, tid;
	double *arr1, *arr2;

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
		#pragma omp parallel 
		{
			t = omp_get_num_threads();
			tid = omp_get_thread_num();
			if(tid==0)
				printf("\nThreads: %d", t);
		}


	}

	arr1 = (double*)malloc(FIXED_SZ * sizeof(double));
	arr2 = (double*)malloc(FIXED_SZ * sizeof(double));
	
	if((arr1 == NULL) || (arr2 ==NULL)){
		printf("\nCould not allocate memory\n");
		exit(0);
	}


	//Initialize
	for(i=0; i < FIXED_SZ; i++){
		arr1[i] = (double)(i) + 0.125;
	}
	
	printf("\n Rank %d is done initializing", myrank);
	
	start=MPI_Wtime();

	//Copy
	for(it = 0; it < ITERS ; it++){
		#pragma omp parallel private(j)
		for(i=0; i<mychunk; i++){
			for(j=0; j < FIXED_SZ; j++){
				arr2[j] = arr1[(FIXED_SZ-1) - j];
			}
		}
	}

	for(it=0; it < COMM_ITERS; it++){
 		MPI_Alltoall(sendbuf, MSG_SZ, MPI_DOUBLE, recvbuf, MSG_SZ, MPI_DOUBLE, MPI_COMM_WORLD);
	 	MPI_Alltoall(sendbuf, MSG_SZ, MPI_DOUBLE, recvbuf, MSG_SZ, MPI_DOUBLE, MPI_COMM_WORLD);
	}

	for(it=0; it < COMM2_ITERS; it++){
 		MPI_Alltoall(sendbuf, MSG_SZ, MPI_DOUBLE, recvbuf, MSG_SZ, MPI_DOUBLE, MPI_COMM_WORLD);
	}

	end=MPI_Wtime();

	free(arr1);
	free(arr2);

	printf("\nTime is: %lf secs\n", end-start);

	MPI_Finalize();

return 0;
}
