#include "include/lu_openmp.hpp"
#include "include/lu_pthread.hpp"
#include "include/lu_sequential.hpp"
#include "include/matrix.hpp"
#include <iostream>
#include <cmath>
#include <ctime>
#include <tuple>
#include <pthread.h>
#include <omp.h>

double l2_norm(matrix& m) {
    double norm = 0;
    int n = m.size();
    #pragma omp parallel for collapse(2) reduction(+ : norm)
    for(int i=0;i<n;i++)
        for(int j=0;j<n;j++)
            norm += m.at(i, j) * m.at(i, j);
            
    return sqrt(norm);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage : ./test <size>\n");
        exit(1);
    }
    int n = atoi(argv[1]);
    matrix m(n);

    matrix l(n, 0);
    matrix u(n, 0);
    std::vector<int> p(n, 0);
    double start = omp_get_wtime();
    lu_factorise_pthread(m, l, u, p);
    double end = omp_get_wtime();
    std::cout << "time taken by pthread implementation for n = " 
        << n << " : " << end - start 
        << " s" << std::endl; 

    std::cout << "----Verify Correctness----" << std::endl;

    matrix err = (permute(m, p) - l*u);
    std::cout << "||(PA - LU)|| = " << l2_norm(err) << std::endl;


}
