#include <cstdlib>
#include <cstring>
#include <vector>
#include <tuple>
#include <stdexcept>
#include <cassert>
#include "include/lu_sequential.hpp"
#include <omp.h>

#define __max(a,b) (a) > (b) ? (a) : (b)

inline double dabs(double d) { 
    return d > 0 ? d : -d;
}

/*NOTE : Only pass in matrix l initialized to all 0.0,
         and matrix u initialized to all 0.0
*/
void lu_factorise_omp(matrix a, matrix& l, matrix& u, std::vector<int>& p) {
    int n = a.size();

    // this-for loop parallelization might offer some improvement in performance as we scale n
    #pragma omp parallel for
    for(int i =0;i < n;i++) {
        p[i] = i;
        l.at(i, i) = 1.0;
    }

    // you cannot parallelize here, you need to make kth col upper triangular before proceeding to (k+1)th col
    for(int k=0;k<n;k++) {
        double max = 0.0;
        int k_;
        
        #pragma omp parallel
        {      
            double pmax = 0;
            int k__;
            #pragma omp for schedule(static, 16)
            for(int i=k;i<n;i++) {
                if (pmax < dabs(a.at(i, k))) {
                    pmax = dabs(a.at(i, k));
                    k__ = i;
                }
            }
            #pragma omp critical
            {   
                if (max < pmax) {
                    max = pmax;
                    k_ = k__;
                }
            }
        }

        if (max == 0.0) {
            throw std::invalid_argument("matrix is singular");
        }
        assert(k_ >= k);
        int temp = p[k];
        p[k] = p[k_];     
        p[k_] = temp;

        // deciding not to parallelize the swap 
        /*In a high-performance oriented system, especially with multiple sockets,
         more threads are very important to exploit the available bandwidth.
         But in my machine improvements are not seen*/
        swap_rows(a, k, k_);
        swap_rows(l, k, k_, k);
        u.at(k, k) = a.at(k, k);

        #pragma omp parallel for
        for(int i=k+1;i<n;i++) {
            l.at(i, k) = a.at(i, k) / u.at(k, k);
            u.at(k, i) = a.at(k, i);
        }

        // double start = omp_get_wtime();
        #pragma omp parallel for collapse(2)
        for(int i=k+1;i<n;i++) {
            for(int j=k+1;j<n;j++) {
                a.at(i, j) = a.at(i, j) - l.at(i, k) * u.at(k, j);
            }
        }
        // double end = omp_get_wtime();
        // printf("bottleneck time : %lfs\n", end - start);
    }
}