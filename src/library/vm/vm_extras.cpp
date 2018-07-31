/*
Copyright (c) 2016 Microsoft Corporation. All rights reserved.
Released under Apache 2.0 license as described in the file LICENSE.

Author: Keeley Hoek
*/
#include <cstdio>
#include <vector>
#include "library/vm/vm_nat.h"
#include "library/vm/vm_io.h"
#include "library/vm/vm_extras.h"

namespace lean {

static std::vector<const name *> extra_names;
static std::vector<vm_obj> extra_ios;

// The "DECLARE_VM_BUILTIN" macro must be invoked with constant arguments, so we do
// what must be done!
#define register_extra_pure(strarr, func) do { \
    const name *__re_name = new name(strarr);  \
    DECLARE_VM_BUILTIN(*__re_name, func);      \
    extra_names.push_back(__re_name);          \
} while(0)

// Order matters for these!
#define register_extra_io(func) do { \
    extra_ios.push_back(mk_native_closure(func)); \
} while(0)

bool is_extra_name(name const &n) {
    for(name const *en : extra_names) {  
        if(n == *en) {
            return true;
        }
    }
    return false;
}

static vm_obj vm_extra_io_impl() {
    return mk_vm_constructor(0, extra_ios.size(), extra_ios.data());
}

static void install_extra_ios() {
    register_extra_pure(name("vm_extra_io_impl"), vm_extra_io_impl);
}

//Pure function declarations

static vm_obj extras_find_separating_hyperplane(vm_obj const & _dim,
    vm_obj const & _vects) {
    size_t dim = force_to_size_t(_dim);
    fprintf(stderr, "FIXME Find separating hyperpane!");
    return mk_vm_nat(dim);
}

//IO function declarations --- they all need an unnamed last argument, e.g.

static vm_obj extras_nop(vm_obj const &) {
    return mk_io_result(mk_vm_unit());
}

static vm_obj extras_greet(vm_obj const &) {
    fprintf(stderr, "Hello, world!\n");
    return mk_io_result(mk_vm_unit());
}

void initialize_vm_extras() {
    //Pure function declarations
    register_extra_pure(name({"extras", "find_separating_hyperplane"}),
        extras_find_separating_hyperplane);

    //IO function declarations --- order MUST be as specified in extras.lean
    //WARNING: due to a "feature"(?!?) of the VM, there must be **at least** TWO
    //declared IO functions (else some nonsense with compound expressions happens).
    register_extra_io(extras_nop);
    register_extra_io(extras_greet);
    
    //Install the IO implementation, must be called after all of the calls to 
    //register_extra_io().
    install_extra_ios();
}

void finalize_vm_extras() {
    extra_names.clear();
}

}
