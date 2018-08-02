#pragma once

#include <cstdio>
#include <unistd.h>

// #define SVM_DEBUG

#ifdef SVM_DEBUG
#define LIBSVM_BUG_ON(pred) do { if(pred) { fprintf(stderr, "SVMERR: bug in \"%s\" on line %d\n", __FILE__, __LINE__); exit(1); } } while(0)
#else
#define LIBSVM_BUG_ON(pred) do { } while(0)
#endif

#include "lib_libsvm.h"

struct svm_instance {
    unsigned dim;

    struct svm_parameter params;
    struct svm_problem prob;
    struct svm_node *x_space;

    unsigned next_vect;
    unsigned next_component;
    unsigned next_node;

    struct svm_model *model;
};

void ksvm_init_instance(struct svm_instance *inst, unsigned dim, unsigned nvects);
void ksvm_destroy_instance(struct svm_instance *inst);

const char * ksvm_check_instance(struct svm_instance *inst);
bool ksvm_run_instance(struct svm_instance *inst);

void ksvm_get_hyperplane(struct svm_instance *inst, double *normal, double *off);

//vectors can either be red (+1) or blue (-1)
static __attribute__((unused)) inline void start_vector(struct svm_instance *inst, bool red) {
    LIBSVM_BUG_ON(inst->prob.l <= inst->next_vect);

    inst->prob.y[inst->next_vect] = red ? +1 : -1;
    inst->prob.x[inst->next_vect] = inst->x_space + inst->next_node;
    inst->next_component = 0;
    inst->next_vect++;
}

static __attribute__((unused)) inline void end_vector(struct svm_instance *inst) {
    inst->x_space[inst->next_node].index = -1;
    inst->next_node++;
}

//vectors can either be red (+1) or blue (-1)
static __attribute__((unused)) inline void store_component(struct svm_instance *inst, double value) {
    LIBSVM_BUG_ON(inst->dim <= inst->next_component);

    inst->x_space[inst->next_node].index = 1 + inst->next_component++;
    inst->x_space[inst->next_node].value = value;

    inst->next_node++;
}