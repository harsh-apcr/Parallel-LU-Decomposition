#include "include/lu_openmp.hpp"
#include "include/lu_pthread.hpp"
#include "include/lu_sequential.hpp"
#include "include/matrix.hpp"
#include <iostream>
#include <cmath>
#include <ctime>
#include <tuple>

double l2_norm(matrix& m) {
    double norm = 0;
    int n = m.size();
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
    clock_t start, end;
    start = clock();
    lu_factorise_seq(m, l, u, p);
    end = clock();
    std::cout << "time taken by sequential implementation for n = " 
        << n << " : " << double(end - start) / double(CLOCKS_PER_SEC) 
        << " s" << std::endl; 

    std::cout << "Do you want to verify correctness(y/n) ? ";

    if (getchar() == 'y') {
        std::cout << "----Verifying Correctness----" << std::endl;
        std::cout << "this may take some time...... as the verification part is not optimized" << std::endl;
        matrix err = (permute(m, p) - l*u);
        std::cout << "||(PA - LU)|| = " << l2_norm(err) << std::endl;
    }


}
