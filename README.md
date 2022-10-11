# LU Decomposition Implementation with OpenMP and PThread
# Harsh Sharma
# 2019MT60628

## Introduction

In this report an approach for parallelizing the sequential algorithm for LU factorisation of an `n x n` square matrix (given in the problem statement) using OpenMP and PThreads is presented. The LU decomposition algorithm uses gaussian elimination of a dense `n x n` matrix with partial pivoting to factor into a lower triangular times an upper triangular matrix. In matrix computations, pivoting involves finding the largest magnitude value in a row, column, or both and then interchanging rows and/or columns in the matrix for the next step in the algorithm. The purpose of pivoting is to reduce round-off error, which enhances numerical stability.

## How to use this library ?

All the source code is contained in the zip file `2019MT60628_A2.zip`. 
You can compile the program by typing `make` in the project directory which produces three executables `test_seq`, `test_omp`, `test_pthread`, these files are the one you interact with.

#### `test_seq`
`usage : .\test_seq <size>`

This file can be executed using the above syntax on the terminal and `<size>` is the size of the matrix to generate using `drand48()` as uniform random number generator for which it computes the LU-decomposition.
The file also computes the time taken in computing the LU decomposition of the matrix, and prompts the user for verifying the result (i.e. verifying outputs **P**, **U**, and **L**)

Example usage : 
```terminal
$ .\test_seq 1000
time taken by sequential implementation for n = 1000 : 7.22163 s
Do you want to verify correctness(y/n) ?   # press y here to see the L2 norm of the residual
----Verifying Correctness----
this may take some time...... as the verification part is not optimized
||(PA - LU)|| = 1.44233e-12
```

#### `test_omp` or `test_pthread`

`usage : .\test_omp <size> <num_thread>`

`usage : .\test_omp <size> <num_thread>`

These two files does the same thing as `test_seq` just that, both of them use parallel routines to do their computations and the number of threads spawned by the routines are specified by `<num_thread>`

Example usage :
```terminal
$ ./test_omp 1000 8
time taken by omp implementation for n = 1000 : 1.64817 s
Do you want to verify correctness(y/n) ? # press y here
----Verifying Correctness----
this may take some time...... as the verification part is not optimized
||(PA - LU)|| = 1.44233e-12
```

## OpenMP Implementation

1. The initializations of $$\pi, L, U$$ were parallelized
2. Outer k loop (of the pseudo-code in the problem statement cannot be parallelized as result of (k+1)st iterations depends on result of kth iterations and they're being modified each iteration)

3. ```
    for i = k to n
        if max < |a(i,k)|
          max = |a(i,k)|
          k' = i
    ```
    This for loop is parallelized, since `max` and `k'` are shared variables, a critical section is maintained that resolves race condition. Also to prevent executing under critical section _O(n^2)_ time, thread private `max` and `k'` variables are maintained such that finally while aggregating the result critical section is executed only |NUM_THREADS| many times (look at `lu_openmp.cpp` line no. 34-52 for more details)

4. It is decided not to parallelize the swapping of rows. `memcpy`(see UNIX MANUAL) is used to do the swapping and it is IO-bound, not CPU-bound. Splitting it into multiple threads isn't going to help, even if the threads run simultaneously on different CPU cores, those cores will just be competing with each other for memory bandwidth. Though in high performance server systems with many IO channels, a single thread may not be able to saturate the bandwidth.

5. Finally last two for loops are trivially parallelized by just using `#pragma omp parallel for`


## PThread Implementation

The logic of pthread implementation is exactly similar to OpenMP implementation, just that for loop are parallelized using static block/chunk splitting, critical section is implemented using `pthread_mutex_t` (see UNIX MANUAL) which locks a particular code section and implicit barriers of `#pragma omp ...` are implemented using `pthread_barrier_t` (SEE UNIX MANUAL), calling `pthread_barrier_wait()` has the same effect as `#pragma omp barrier` or other implicit OpenMP barrier;

## Benchmarks of the three implementations

All the results were ran on a Zorin OS (Linux Distro) with Intel® Core™ i5-8250U CPU @ 1.60GHz × 8, 15.5 GiB Memory.

### Sequential Implementation

| n | time | 
| --- | --- |
| 100 | 0.01077s |
| 500   | 0.90066s |
|  1000 | 7.20221s |
|  1500 | 24.2687s  |
|   2000 | 57.9258s |
| 3000 | 196.048s |
| 4000 | 463.067s |
| 5000 |  905.549s |
| 6000 |  1555.823s |
| 7000 | 2470.58s |

## OpenMP and PThread Implementation


| n | num_threads | time(OpenMP) | time(PThread) |
| --- | ---- | ---| --- |
| 100 |  2   |0.0064007s  |    0.075223s      |
| 100 |  4   |  0.0079250s  |  0.064856s     |
| 100 |  8   |  0.0038186s |    0.117196s    |
|  100 |  16 |  0.0314944s   |  0.174482s    |

| n | num_threads | time(OpenMP) | time(PThread) |
| --- | ---- | ---| --- |
|  1000  |    2   |  3.86927s     |   3.89122s      |
|  1000  |    4   |  1.92353s     |  2.04741s       |
|  1000  |    8   |   1.67508s    |  1.98652s        |
|  1000  |    16   |   1.96554s   |  2.81689s       |

| n | num_threads | time(OpenMP) | time(PThread) |
| --- | ---- | ---| --- |
| 1500  |  2 |   12.9034s   |   12.8229s            |
| 1500  |  4 |   6.48015s   |   6.85625s             |
| 1500  |  8 |   5.7488s   |  6.11028s              |
| 1500  |  16  |  7.07978s  |    7.69341s           |

| n | num_threads | time(OpenMP) | time(PThread) |
| --- | ---- | ---| --- |
| 2000  | 2  |  30.2288s | 30.0581s       |
| 2000  | 4  |  16.4312s |16.5827s      |
| 2000  | 8  |  15.6701s |   14.8203s               |
| 2000  | 16  | 17.2524s  | 17.2351s        |

| n | num_threads | time(OpenMP) | time(PThread) |
| --- | ---- | ---| --- |
| 3000  | 2  | 102.582s  |  104.382s             |
| 3000  | 4  | 60.0153s  |  57.3816s    |       
| 3000  | 8  | 53.6763s  |     54.4344s     |       
| 3000  | 16  | 55.7653 s  |   57.3233s        |       

| n | num_threads | time(OpenMP) | time(PThread) |
| --- | ---- | ---| --- |
| 4000  | 2  | 260.423s  |  265.232s          |
| 4000  | 4  | 147.141s   |  142.413s  |       
| 4000  | 8  | 138.068s  |     133.862s    |       
| 4000  | 16  | 140.790s  |    138.238s              |   

for `n = 7000`

| n | num_threads | time(OpenMP) | time(PThread) |
| --- | ---- | ---| --- |
| 7000  | 8  |  958.079s |      1140.91s     |       


SEE THE JUPYTER NOTEBOOK FILE FOR EFFICIENCY PLOT