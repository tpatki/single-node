
bmark3:
	mpicc -O0 -openmp -o bmark3 bmark3.c -Xlinker -rpath=$(PATH_LIBMSR) -L$(PATH_LIBMSR) -lrapl

clean:
	rm -f bmark3 *.o

