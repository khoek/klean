/*
Copyright (c) 2018 Keeley Hoek. All rights reserved.
Released under Apache 2.0 license as described in the file LICENSE.

Author: Keeley Hoek
*/
#include <cstdio>
#include <cstdint>
#include <cmath>

//EPSILON is the "+ h" to use when calculating partial derivatives
#define EPSILON   1e-10
//THRESHOLD is the normsquared of the x0 delta at which we decided we have //converged
#define THRESHOLD 1e-16
//GAMMA is multiplied into the x0 delta before taking it away from x0
#define GAMMA     1e-2

#define MAX_ITERS 10000000

// #define DEBUG

#ifdef DEBUG
# define debug_print_state(dim, x0, g) print_state(dim, x0, g)
# define debug_print_vec(n, ptr) print_vec(n, ptr)
# define dprintf(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
#else
# define NOP do { } while(0)
# define debug_print_state(dim, x0, g) NOP
# define debug_print_vec(n, ptr) NOP
# define dprintf(fmt, ...) NOP
#endif

static inline double normsquared(uint32_t dim, double *v) {
    double ret = 0;
    for(uint32_t i = 0; i < dim; i++) {
        ret += v[i] * v[i];
    }
    return ret;
}

static inline void print_vec(uint32_t dim, double *v) {
    fprintf(stderr, "(");
    for(uint32_t i = 0; i < dim; i++) {
        fprintf(stderr, "%lf%s", v[i], i == dim - 1 ? "" : ", ");
    }
    fprintf(stderr, ")");
}

static inline void print_state(uint32_t dim, double *x0, double *g) {
    print_vec(dim, x0);
    fprintf(stderr, " - d ");
    print_vec(dim, g);
    fprintf(stderr, " : %lf", normsquared(dim, g));
    fprintf(stderr, "\n");
}

static inline void vec_sub_from(uint32_t dim, double *u, double *v) {
    for(uint32_t i = 0; i < dim; i++) {
        u[i] -= v[i];
    }
}

//compute grad of func at x0, multiply grad by gamma, store the answer in g
static inline void calc_delta(uint32_t dim, double gamma, double *x0,
    double (*func)(uint32_t dim, void *data, double *x), void *data, double *g) {
    double val = func(dim, data, x0);
    for(uint32_t i = 0; i < dim; i++) {
        double oldcoord = x0[i];
        x0[i] += EPSILON;
        g[i] = (func(dim, data, x0) - val) * gamma / EPSILON;
        x0[i] = oldcoord;
    }
}

//FIXME add return codes for detected nonconvergence, etc.
int32_t gradient_decent(uint32_t dim, double *x0,
    double (*func)(uint32_t dim, void *data, double *x), void *data) {
    //FIXME detect a failure to converge

    int32_t iterations;

    dprintf("start\n");
    for(iterations = 0; iterations < MAX_ITERS; iterations++) {
        double g[dim];
        calc_delta(dim, GAMMA, x0, func, data, g);
        debug_print_state(dim, x0, g);
        vec_sub_from(dim, x0, g);
        if(normsquared(dim, g) < THRESHOLD) {
            break;
        }
    }
    dprintf("end ");
    debug_print_vec(dim, x0);
    dprintf("\n");

    return iterations == MAX_ITERS ? -1 : iterations;
}