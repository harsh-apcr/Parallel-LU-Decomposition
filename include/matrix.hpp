#include <vector>
#include <cstdlib>

#ifndef _MATRIX_H__
#define _MATRIX_H__

class matrix {
private:
    std::vector<double> flattened_matrix;
    int n;
public:
    matrix(int, double);
    matrix(int);    // initialize using drand48
    matrix(const matrix&);
    double& at(int, int);
    double get(int, int) const;
    double* data();
    const double* ro_data() const;
    int size() const;
    matrix& operator=(matrix&);
    matrix operator-(const matrix&);
    matrix operator*(matrix&);
};

void swap_rows(matrix& a, int k, int k_);
void swap_rows(matrix& a, int k, int k_, size_t num);  // num is number of values swapped in a row (the remaining are intqc)
matrix permute(const matrix& a, std::vector<int>& perm);
void print_matrix(matrix& m);
void print_matrix(matrix&& m);


#endif // _MATRIX_H__