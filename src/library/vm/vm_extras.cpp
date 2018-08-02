/*
Copyright (c) 2018 Keeley Hoek. All rights reserved.
Released under Apache 2.0 license as described in the file LICENSE.

Author: Keeley Hoek
*/
#include <cstdio>
#include <vector>
#include "library/vm/vm_nat.h"
#include "library/vm/vm_int.h"
#include "library/vm/vm_list.h"
#include "library/vm/vm_array.h"
#include "library/vm/vm_io.h"
#include "library/vm/vm_extras.h"
#include "library/vm/lib_decent.h"
#include "library/vm/lib_ksvm.h"

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

// These guys are is off for now---a custom implemention might have been better, but we currently have a
// libsvm implementation, so lets roll with that.

// //Helper function, expects a list of arrays of naturals with each array of length dim
// static std::vector<double *> * unpack_vector_list(unsigned dim, vm_obj const & _vlist) {
//     list<parray<vm_obj>> vlist = to_list_array(_vlist);

//     //FIXME it's a shame that it is better to duplicate the vector list, because
//     //parray isn't just an array.
//     std::vector<double *> *vects = new std::vector<double *>();

//     for(list<parray<vm_obj>>::iterator it = vlist.begin(); it != vlist.end(); it++) {
//         double *v = new double[dim];
//         for(unsigned i = 0; i < dim; i++) {
//             v[i] = (double) force_to_unsigned((*it)[i]);
//         }
//         vects->push_back(v);
//     }
//     return vects;
// }

// static void delete_vector_list(std::vector<double *> *vl) {
//     for(std::vector<double *>::iterator it = vl->begin(); it != vl->end(); it++) {
//         delete *it;
//     }
//     delete vl;
// }

// //FIXME all of this same info is passed on the stack. Is that okay?
// double plane_obj_func(uint32_t dim, void *data, double *x) {
//     std::vector<double *> **v = (std::vector<double *> **) data;
//     std::vector<double *> *va = v[0];
//     std::vector<double *> *vb = v[1];

//     //FIXME use the points in set va, and the points in set vb, to compute the objective
//     //function for the gradient decent algorithm. x is the current state

//     return 1.0;
// }

static void load_svm_vector_list(struct svm_instance *inst, unsigned dim, bool red,
    list<parray<vm_obj>> const & vlist) {
    for(list<parray<vm_obj>>::iterator it = vlist.begin(); it != vlist.end(); it++) {
        start_vector(inst, red);
        for(unsigned i = 0; i < dim; i++) {
            store_component(inst, force_to_unsigned((*it)[i]));
        }
        end_vector(inst);
    }
}

struct vm_array : public vm_external {
    parray<vm_obj> m_array;
    vm_array(parray<vm_obj> const & a):m_array(a) {}
    virtual ~vm_array() {}
    virtual void dealloc() override { this->~vm_array(); get_vm_allocator().deallocate(sizeof(vm_array), this); }
    virtual vm_external * ts_clone(vm_clone_fn const &) override;
    virtual vm_external * clone(vm_clone_fn const &) override { lean_unreachable(); }
};

static vm_obj extras_find_separating_hyperplane(vm_obj const & _dim, vm_obj const & _vlist_a,
    vm_obj const & _vlist_b) {
    unsigned dim = force_to_unsigned(_dim);

    list<parray<vm_obj>> vlist_a = to_list_array(_vlist_a);
    list<parray<vm_obj>> vlist_b = to_list_array(_vlist_b);

    struct svm_instance inst;
    ksvm_init_instance(&inst, dim, length(vlist_a) + length(vlist_b));

    //FIXME the docs say it is faster (and likely better) if we normalise

    load_svm_vector_list(&inst, dim, true , vlist_a);
    load_svm_vector_list(&inst, dim, false, vlist_b);

    //FIXME remove this
    const char *error_msg = ksvm_check_instance(&inst);
	if(error_msg) {
		fprintf(stderr,"ERROR: %s\n", error_msg);
		exit(1);
	}

    ksvm_run_instance(&inst);

    double normal[dim];
    double rho;
    ksvm_get_hyperplane(&inst, normal, &rho);

    ksvm_destroy_instance(&inst);

#define DECIMALS 1000

    //FIXME there is no need to copy in zeros (but it doesn't really cost anything)
    parray<vm_obj> normal_arr(dim, mk_vm_int(1));
    for(unsigned d = 0; d < dim; d++) {
        normal_arr.set(d, mk_vm_int((int) (DECIMALS * normal[d])));
    }

    return mk_vm_constructor(0, to_obj(normal_arr), mk_vm_int((int) (rho * DECIMALS)));
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
