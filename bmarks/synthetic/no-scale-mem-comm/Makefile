
bmark4:
	mpicc -O0 -openmp -o bmark4 bmark4.c -Xlinker -rpath=$(PATH_LIBMSR) -L$(PATH_LIBMSR) -lrapl

clean:
	rm -f bmark4 *.o

