#ifndef _LU_PTHREAD__
#define _LU_PTHREAD__

#include "matrix.hpp"
#include <vector>
extern int NUM_THREADS;
// define your function prototypes here
void lu_factorise_pthread(matrix a, matrix& l, matrix& u, std::vector<int>& p);

#endif // _LU_PTHREAD__