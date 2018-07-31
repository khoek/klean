/*
Copyright (c) 2017 Microsoft Corporation. All rights reserved.
Released under Apache 2.0 license as described in the file LICENSE.

Author: Leonardo de Moura
*/
#pragma once

#include "library/parray.h"
#include "library/vm/vm.h"
#include "library/vm/vm_list.h"

namespace lean {

parray<vm_obj> const & to_array(vm_obj const & o);
vm_obj to_obj(parray<vm_obj> const & a);

static __attribute__((unused)) list<parray<vm_obj>> to_list_array(vm_obj const & obj) {
    return to_list<parray<vm_obj>>(obj, [](vm_obj const & o) { return to_array(o); });
}

void initialize_vm_array();
void finalize_vm_array();
void initialize_vm_array_builtin_idxs();
}
