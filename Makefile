CC = g++
CFLAGS = -Wall -g
OMPLINKER = -fopenmp

all: test1

test1: test1.cpp lu_openmp.o matrix.o
	$(CC) $(CFLAGS) $(OMPLINKER) -o $@ $^

test: test.cpp lu_sequential.o matrix.o
	$(CC) $(CFLAGS) -o $@ $^

lu_sequential.o: lu_sequential.cpp matrix.o
	$(CC) $(CFLAGS) -c $^ 

matrix.o: matrix.cpp include/matrix.hpp
	$(CC) $(CFLAGS) -c $^

lu_openmp.o: lu_openmp.cpp include/lu_openmp.hpp
	$(CC) $(CFLAGS) $(OMPLINKER) -c $^

lu_pthread.o: lu_pthread.cpp include/lu_pthread.hpp
	$(CC) $(CFLAGS) -c $^

clean: 
	rm *.o test test1 include/*.gch