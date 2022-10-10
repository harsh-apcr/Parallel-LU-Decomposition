CC = g++
CFLAGS = -Wall -g
OMPLINKER = -fopenmp
PLINKER = -lpthread

all: test1

test2: test2.cpp lu_pthread.o matrix.o
	$(CC) $(CFLAGS) $(PLINKER) $(OMPLINKER) -o $@ $^

test1: test1.cpp lu_openmp.o matrix.o
	$(CC) $(CFLAGS) $(OMPLINKER) -o $@ $^

test: test.cpp lu_sequential.o matrix.o
	$(CC) $(CFLAGS) -o $@ $^

lu_sequential.o: lu_sequential.cpp include/lu_sequential.hpp matrix.o
	$(CC) $(CFLAGS) -c $^ 

matrix.o: matrix.cpp include/matrix.hpp
	$(CC) $(CFLAGS) -c $^

lu_openmp.o: lu_openmp.cpp include/lu_openmp.hpp
	$(CC) $(CFLAGS) $(OMPLINKER) -c $^

lu_pthread.o: lu_pthread.cpp include/lu_pthread.hpp
	$(CC) $(CFLAGS) $(PLINKER) -c $^

clean: 
	rm *.o test test1 test2 include/*.gch