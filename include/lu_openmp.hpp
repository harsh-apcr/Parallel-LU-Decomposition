#ifndef _LU_OPENMP__
#define _LU_OPENMP__

#include "matrix.hpp"

// define your function prototypes here
void lu_factorise_omp(matrix a, matrix& l, matrix& u, std::vector<int>& p);

#endif // _LU_OPENMP__