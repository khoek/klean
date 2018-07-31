/*
Copyright (c) 2018 Keeley Hoek. All rights reserved.
Released under Apache 2.0 license as described in the file LICENSE.

Author: Keeley Hoek
*/
#pragma once

#include <cstdint>

int32_t gradient_decent(uint32_t dim, double *x0,
    double (*func)(uint32_t dim, void *data, double *x), void *data);