#include <cstdlib>
#include <cstring>
#include <vector>
#include <tuple>
#include <stdexcept>
#include <cassert>
#include <ctime>
#include "include/lu_openmp.hpp"

inline double dabs(double d) { 
    return d > 0 ? d : -d;
}

/*NOTE : Only pass in matrix l initialized to all 0.0,
         and matrix u initialized to all 0.0
*/
void lu_factorise_seq(matrix a, matrix& l, matrix& u, std::vector<int>& p) {
    int n = a.size();
    clock_t start, end;
    for(int i =0;i < n;i++) 
        p[i] = i;
    for(int i=0;i<n;i++) 
        l.at(i, i) = 1.0;

    for(int k=0;k<n;k++) {
        double max = 0.0;
        int k_;
        
        for(int i=k;i<n;i++) {
            if (max < dabs(a.at(i, k))) {
                max = dabs(a.at(i, k));
                k_ = i;
            }
        }
        

        if (max == 0.0) {
            throw std::invalid_argument("matrix is singular");
        }
        assert(k_ >= k);

        int temp = p[k];
        p[k] = p[k_];     
        p[k_] = temp;

        swap_rows(a, k, k_);
        swap_rows(l, k, k_, k);
        
        u.at(k, k) = a.at(k, k);

        for(int i=k+1;i<n;i++) {
            l.at(i, k) = a.at(i, k) / u.at(k, k);
            u.at(k, i) = a.at(k, i);
        }
        
        start = clock();
        for(int i=k+1;i<n;i++) {
            for(int j=k+1;j<n;j++) {
                a.at(i, j) = a.at(i, j) - l.at(i, k) * u.at(k, j);
            }
        }
        end = clock();
        printf("time taken : %lf\n", double(end - start) / double(CLOCKS_PER_SEC));

    }   
}