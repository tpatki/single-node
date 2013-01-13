//Tapasya Patki
//Synthetic - 3
//Memory-bound and scalable

#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

//Large is 2^32 atm
#define LARGE 4096
#define ITERS 2
//Total array size = 65536 * 8 bytes; 512 MB
//#define FIXED_SZ 131072
//#define FIXED_SZ 16777216
#define FIXED_SZ (20 * 1024 * 1024 * 16)

int main(int argc, char * argv[]){

	int myrank, numnodes;
	unsigned long mychunk;
	int it;
	unsigned long i,j;
	double start, end;
	int t, tid;
	double *arr1, *arr2;

	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	MPI_Comm_size(MPI_COMM_WORLD, &numnodes);
	
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
	
	end=MPI_Wtime();

	free(arr1);
	free(arr2);

	printf("\nTime is: %lf secs\n", end-start);

	MPI_Finalize();

return 0;
}
