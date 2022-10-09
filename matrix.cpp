#include "include/lu_sequential.hpp"
#include <stdexcept>
#include <iostream>
#include <vector>
#include <cassert>
#include <cstring>
/*
Class member function/ constructor definitions
*/
matrix::matrix(int n, double init_value) : flattened_matrix(n * n, init_value), n(n) {}

matrix::matrix(int n) : flattened_matrix(n*n, 0), n(n) {
    //initialize vector using drand48
    srand48(0);
    for(int i =0;i<n;i++)
        for(int j=0;j<n;j++)
            this->flattened_matrix[i * n + j] = drand48(); // number in [0, 1)
}

matrix::matrix(const matrix& m) : flattened_matrix(m.size()*m.size(), 0), n(m.size()) {
    for(int i=0;i<n;i++)
        for(int j=0;j<n;j++)
            flattened_matrix[i * n + j] = m.get(i, j);
}

double& matrix::at(int i, int j) {
    return flattened_matrix[i * n + j];
}

double matrix::get(int i, int j) const {
    return flattened_matrix[i * n + j];
}

const double* matrix::ro_data() const {
    return static_cast<const double*>(flattened_matrix.data());
}

double* matrix::data() {
    return flattened_matrix.data();
}

inline int matrix::size() const {
    return n;
}

// copy constructor via assignment
matrix& matrix::operator=(matrix& m) {
    this->flattened_matrix = m.flattened_matrix;
    this->n = m.n;
    return *this;
}

matrix matrix::operator-(const matrix& m) {
    int n = m.size();
    assert(this->size() == n);
    matrix res(n, 0);
    for(int i=0;i<n;i++)
        for(int j=0;j<n;j++)
            res.at(i, j) = this->at(i, j) - m.get(i, j);
    return res;
}

matrix matrix::operator*(matrix& m) {
    int n = m.size();
    assert(this->size() == n);
    matrix res(n, 0);
    for(int i=0;i<n;i++)
        for(int j=0;j<n;j++)
            for(int k=0;k<n;k++)
                res.at(i, j) += this->at(i, k) * m.at(k, j);
    return res; 
}

void swap_rows(matrix& a, int k, int k_) {
    int n = a.size();
    std::vector<double> temp(a.data() + k * n, a.data() + (k + 1) * n);
    memcpy(a.data() + k * n, a.data() + k_ * n, n * sizeof(double));
    memcpy(a.data() + k_ * n, temp.data(), n * sizeof(double));
}

void swap_rows(matrix& a, int k, int k_, size_t num) {
    int n = a.size();
    std::vector<double> temp(a.data() + k * n, (a.data() + k * n) + num);
    memcpy(a.data() + k * n, a.data() + k_ * n, num * sizeof(double));
    memcpy(a.data() + k_ * n, temp.data(), num * sizeof(double));
}

matrix permute(const matrix& m, std::vector<int>& perm) {
    int n = m.size();
    matrix res(n, 0);
    for(int i=0;i<n;i++) {
        int j = perm[i];
        memcpy(res.data() + i * n, m.ro_data() + j * n, n * sizeof(double));
    }
    return res;
}

void print_matrix(matrix& m) {
    int n = m.size();
    for(int i =0;i<n;i++) {
        for(int j=0;j<n;j++) {
            std::cout << m.at(i, j) << ' ';
        }
        std::cout << std::endl;
    }
}

void print_matrix(matrix&& m) {
    int n = m.size();
    for(int i =0;i<n;i++) {
        for(int j=0;j<n;j++) {
            std::cout << m.at(i, j) << ' ';
        }
        std::cout << std::endl;
    }
}