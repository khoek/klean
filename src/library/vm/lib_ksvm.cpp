#include <cstdio>
#include <cstdlib>
#include <cstdbool>
#include <cstring>
#include <unistd.h>

#include "lib_ksvm.h"

static void fill_params(struct svm_parameter *params, unsigned dim) {
    params->svm_type = C_SVC;
    params->kernel_type = LINEAR; //RBF;
    params->degree = 3;
    params->gamma = 1.0 / ((double) dim);
    params->coef0 = 0;
    params->nu = 0.5;
    params->cache_size = 100;
    params->C = 100;
    params->eps = 1e-3;
    params->p = 0.1;
    params->shrinking = 1;
    params->probability = 0;
    params->nr_weight = 0;
    params->weight_label = NULL;
    params->weight = NULL;
}

void ksvm_init_instance(struct svm_instance *inst, unsigned dim, unsigned nvects) {
    inst->dim = dim;

    fill_params(&inst->params, dim);
    inst->prob.l = nvects;
    inst->prob.y = new double[nvects];
    inst->prob.x = new svm_node *[nvects];
    inst->x_space = new svm_node[nvects * (dim + 1)];

    inst->next_vect = 0;
    inst->next_node = 0;

    inst->model = NULL;

    //We don't call this, and instead disable logging at compile-time.
    //svm_set_print_string_function(nothing_func);
}

void ksvm_destroy_instance(struct svm_instance *inst) {
    //This internally checks whether inst->model == NULL
    //and yes, it does actually take a pointer pointer... not cool
    svm_free_and_destroy_model(&inst->model);
    svm_destroy_param(&inst->params);
    free(inst->prob.y);
    free(inst->prob.x);
    free(inst->x_space);
}

const char * ksvm_check_instance(struct svm_instance *inst) {
    return svm_check_parameter(&inst->prob, &inst->params);
}

bool ksvm_run_instance(struct svm_instance *inst) {
    inst->model = svm_train(&inst->prob, &inst->params);
    return inst->model != NULL;
}

void ksvm_get_hyperplane(struct svm_instance *inst, double *normal, double *off) {
    unsigned dim = inst->dim;
    struct svm_model *model = inst->model;

    int correction = model->label[0] == -1 ? -1 : 1;
    *off = *model->rho * -1 * correction; //rho is negative the offset

    memset(normal, 0, dim * sizeof(double)); //This is undefined behaviour (give me a break ;))
    for(unsigned d = 0; d < dim; d++) {
        for(int n = 0; n < model->l; n++) {
            normal[d] += model->sv_coef[0][n] * model->SV[n][d].value;
        }
        normal[d] *= correction;
    }
}