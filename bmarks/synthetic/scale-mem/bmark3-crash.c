//Tapasya Patki
//Synthetic - 3
//Mem-bound and scalable

#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>


//LLC_SZ is 20 MB
#define ITERS 2
//This is not 20MB atm
#define LLC_SZ 1310720
//Upto 32 nodes -- ensure that it just fits into L3 cache.
#define (LLC_SZ)

int main(int argc, char * argv[]){

	int myrank, numnodes;
	int it;
	unsigned long i,j,mychunk,smallchunk;
	double start, end;
	int t;

	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	MPI_Comm_size(MPI_COMM_WORLD, &numnodes);


	start=MPI_Wtime();

	if(myrank == 0){
		mychunk = (unsigned long) (LARGE_SZ/numnodes);
		printf("\n Chunk size: %ld", mychunk);
	 #pragma omp parallel
        	 {
			 int tid=omp_get_thread_num();
			 if(tid == 0){
                 	   t =  omp_get_num_threads();
			  }
		 }
	 
		smallchunk = (unsigned long)mychunk/t;
	}
				 	
	if(myrank == 0){
		printf("\n SmallChunk size: %ld", smallchunk);
		printf("\n Threads: %d", t);
	}

	for(it=0;it < ITERS; it++){

	#pragma omp parallel private(i,arrA,arrB)
                 for(i=0; i<t; i++){
			//do some memory intensive operations.
			unsigned long *arrA = (unsigned long*) malloc(smallchunk * sizeof(unsigned long));
			unsigned long *arrB = (unsigned long*) malloc(smallchunk * sizeof(unsigned long));
			
			if(arrA == NULL || arrB == NULL){
				printf("\n Could not allocate memory");
				exit(1);
			}
				

			for(j=0;j<smallchunk;j++){
				arrA[j] = j;			
				arrB[j] = arrA[j];
			}
		
			free(arrA);
			free(arrB);
					
		}
			 
	}

	end=MPI_Wtime();

	printf("\nTime is: %lf secs", end-start);

	MPI_Finalize();

return 0;
}
