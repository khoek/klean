/*
Copyright (c) 2018 Keeley Hoek. All rights reserved.
Released under Apache 2.0 license as described in the file LICENSE.

Author: Keeley Hoek
*/
#pragma once
#include <string>
#include "library/vm/vm.h"

namespace lean {
bool is_extra_name(name const & n);

void initialize_vm_extras();
void finalize_vm_extras();
}
