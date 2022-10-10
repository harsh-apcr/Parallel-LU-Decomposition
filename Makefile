CC = g++
CFLAGS = -Wall -g
OMPLINKER = -fopenmp
PLINKER = -lpthread

all: test_seq test_omp test_pthread

test_pthread: test_pthread.cpp lu_pthread.o matrix.o
	$(CC) $(CFLAGS) $(PLINKER) $(OMPLINKER) -o $@ $^

test_omp: test_omp.cpp lu_openmp.o matrix.o
	$(CC) $(CFLAGS) $(OMPLINKER) -o $@ $^

test_seq: test_seq.cpp lu_sequential.o matrix.o
	$(CC) -o $@ $^

lu_sequential.o: lu_sequential.cpp include/lu_sequential.hpp
	$(CC) $(CFLAGS) -c $^ 

matrix.o: matrix.cpp include/matrix.hpp
	$(CC) $(CFLAGS) -c $^

lu_openmp.o: lu_openmp.cpp include/lu_openmp.hpp
	$(CC) $(CFLAGS) $(OMPLINKER) -c $^

lu_pthread.o: lu_pthread.cpp include/lu_pthread.hpp
	$(CC) $(CFLAGS) $(PLINKER) -c $^

clean: 
	rm *.o test_seq test_omp test_pthread include/*.gch