#ifndef _LU_SEQUENTIAL__
#define _LU_SEQUENTIAL__

#include "matrix.hpp"
#include <tuple>
#include <vector>

void lu_factorise_seq(matrix a, matrix& l, matrix& u, std::vector<int>& p);

#endif // _LU_SEQUENTIAL__