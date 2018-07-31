#pragma once

#include <cstdint>

int32_t gradient_decent(uint32_t dim, double *x0,
    double (*func)(uint32_t dim, void *data, double *x), void *data);