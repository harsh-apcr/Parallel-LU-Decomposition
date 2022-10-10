#include <cstdlib>
#include <cstring>
#include <vector>
#include <tuple>
#include <stdexcept>
#include <cassert>
#include <ctime>
#include "include/matrix.hpp"
#include "include/lu_pthread.hpp"
#include <pthread.h>

pthread_barrier_t barrier_fun2;
pthread_mutex_t mutex_fun2;

inline double dabs(double d) { 
    return d > 0 ? d : -d;
}

struct struct1 {
    std::vector<int>* p;
    matrix* l;
    int tid;
};

struct struct2 {
    matrix* a;
    double* max;
    int* k_;
    int k;
    int tid;
};

struct struct3 {
    matrix *a;
    matrix *u;
    matrix *l;
    int k;
    int tid;
};

void* parallel_fun1(void *args) {
    struct struct1 s = *reinterpret_cast<struct struct1*>(args);
    std::vector<int>& p = *s.p;
    matrix& l = *s.l;
    int tid = s.tid;
    int n = p.size();
    int from = tid * (n / NUM_THREADS);
    int to = (tid == NUM_THREADS - 1) ? n : (tid + 1) * (n / NUM_THREADS);
    for(int i =from;i < to;i++) {
        p[i] = i;
        l.at(i, i) = 1.0;
    }
    return NULL;
}


void* parallel_fun2(void *args) {
    struct struct2 s = *reinterpret_cast<struct struct2*>(args);
    matrix& a = *s.a;
    double& max = *s.max;
    int& k_ = *s.k_;

    int k = s.k;
    int tid = s.tid;
    int n = a.size();

    double pmax = 0;
    int k__;
    int from = k + tid * ((n- k)/ NUM_THREADS);
    int to = (tid == NUM_THREADS - 1) ? n : k + (tid + 1) * ((n-k)/NUM_THREADS);
    for(int i=from;i<to;i++) {
        if (pmax < dabs(a.at(i, k))) {
            pmax = dabs(a.at(i, k));
            k__ = i;
        }
    }
    pthread_barrier_wait(&barrier_fun2);
    pthread_mutex_lock(&mutex_fun2);
    if (max < pmax) {
        max = pmax;
        k_ = k__;
    }
    pthread_mutex_unlock(&mutex_fun2);
    return NULL;
}

void* parallel_fun3(void *args) {
    struct struct3 s = *reinterpret_cast<struct struct3*>(args);
    matrix& a = *s.a;
    matrix& u = *s.u;
    matrix& l = *s.l;
    int k = s.k;
    int tid = s.tid;
    int n = a.size();

    int from = k + 1 + tid * ((n- k -1)/ NUM_THREADS);
    int to = (tid == NUM_THREADS - 1) ? n : k + 1 + (tid + 1) * ((n-k-1)/NUM_THREADS);
    for(int i=from;i<to;i++) {
        l.at(i, k) = a.at(i, k) / u.at(k, k);
        u.at(k, i) = a.at(k, i);
    }

    return NULL;
}

void* parallel_fun4(void* args) {
    struct struct3 s = *reinterpret_cast<struct struct3*>(args);
    matrix& a = *s.a;
    matrix& u = *s.u;
    matrix& l = *s.l;
    int k = s.k;
    int tid = s.tid;
    int n = a.size();

    int from = k + 1 + tid * ((n- k -1)/ NUM_THREADS);
    int to = (tid == NUM_THREADS - 1) ? n : k + 1 + (tid + 1) * ((n-k-1)/NUM_THREADS);
    
    for(int i=from;i<to;i++) {
        for(int j=k+1;j<n;j++) {
            a.at(i, j) = a.at(i, j) - l.at(i, k) * u.at(k, j);
        }
    }

    // int chunk_size = 16;
    // int from = k+1+tid*chunk_size;
    // for(int i=from;i<n;i+=NUM_THREADS * chunk_size) {
    //     for(int r=i;r<i+chunk_size;r++) {
    //         for(int j=k+1;j<n;j++) {
    //             a.at(r, j) -= l.at(r, k) * u.at(k, j);
    //         }
    //     }
    // }
    return NULL;
}

/*NOTE : Only pass in matrix l initialized to all 0.0,
         and matrix u initialized to all 0.0
*/
void lu_factorise_pthread(matrix a, matrix& l, matrix& u, std::vector<int>& p) {

    int n = a.size();

    pthread_t threads[NUM_THREADS]; // only possible in newer versions of C++ compiler
    struct struct1 args1[NUM_THREADS];
    for(int i=0;i<NUM_THREADS;i++) {
        args1[i].p = &p;
        args1[i].l = &l;
        args1[i].tid = i;
        pthread_create(&threads[i], NULL, parallel_fun1, &args1[i]);
    }

    for(int i=0;i<NUM_THREADS;i++) {
        pthread_join(threads[i], NULL);
    }

    for(int k=0;k<n;k++) {
        double max = 0.0;
        int k_;
        pthread_barrier_init(&barrier_fun2, NULL, NUM_THREADS);
        pthread_mutex_init(&mutex_fun2, NULL);
        struct struct2 args2[NUM_THREADS];
        for(int i=0;i<NUM_THREADS;i++) {
            args2[i].k_ = &k_;args2[i].max = &max;args2[i].tid = i;args2[i].a = &a;args2[i].k = k;
            pthread_create(&threads[i], NULL, parallel_fun2, &args2[i]);
        }

        for(int i=0;i<NUM_THREADS;i++) {
            pthread_join(threads[i], NULL);
        }

        pthread_barrier_destroy(&barrier_fun2);
        pthread_mutex_destroy(&mutex_fun2);


        if (max == 0.0) {
            throw std::invalid_argument("matrix is singular");
        }
        assert(k_ >= k);

        int temp = p[k];
        p[k] = p[k_];     
        p[k_] = temp;

        // choosing not to parallelize memcpy
        swap_rows(a, k, k_);
        swap_rows(l, k, k_, k);

        u.at(k, k) = a.at(k, k);

        struct struct3 args3[NUM_THREADS];
        for(int i=0;i<NUM_THREADS;i++) {
            args3[i].a = &a;args3[i].u = &u;args3[i].l = &l;args3[i].tid = i;args3[i].k = k;
            pthread_create(&threads[i], NULL, parallel_fun3, &args3[i]);
        }

        for(int i=0;i<NUM_THREADS;i++) {
            pthread_join(threads[i], NULL);
        }

        // clock_t start, end;
        // start = clock();
        for(int i=0;i<NUM_THREADS;i++) {
            args3[i].a = &a;args3[i].u = &u;args3[i].l = &l;args3[i].tid = i;args3[i].k = k;
            pthread_create(&threads[i], NULL, parallel_fun4, &args3[i]);
        }

        for(int i=0;i<NUM_THREADS;i++) {
            pthread_join(threads[i], NULL);
        }
        
        
        // end = clock();
        // printf("bottleneck time : %lf\n", double(end - start) / double(CLOCKS_PER_SEC));
    }
}