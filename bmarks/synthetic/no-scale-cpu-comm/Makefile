
bmark2:
	mpicc -O0 -openmp -o bmark2 bmark2.c -Xlinker -rpath=$(PATH_LIBMSR) -L$(PATH_LIBMSR) -lrapl

clean:
	rm -f bmark2 *.o

