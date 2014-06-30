local env    = environment()
local l      = param_univ("l")
local nat    = Const("nat")
local real   = Const("real")
local one    = Const("one")
local Ul     = mk_sort(l)
local lst_l  = Const("lst", {l})
local vec_l  = Const("vec", {l})
local mat_l  = Const("mat", {l})
local A      = Local("A", Ul)
local n      = Local("n", nat)
local ll     = Local("ll", lst_l(A))
local len_l  = Const("len", {l})
local lst_1  = Const("lst", {1})
local l1     = param_univ("l1")
local l2     = param_univ("l2")
local m      = Local("m", nat)
env = add_decl(env, mk_var_decl("nat", Type))
env = add_decl(env, mk_var_decl("real", Type))
env = add_decl(env, mk_var_decl("one", nat))
env = add_decl(env, mk_var_decl("lst", {l}, mk_arrow(Ul, Ul)))
env = add_decl(env, mk_var_decl("len", {l}, Pi(A, mk_arrow(lst_l(A), nat))))
env = add_decl(env, mk_var_decl("vec", {l}, mk_arrow(Ul, nat, Ul)))
env = add_decl(env, mk_var_decl("mat", {l}, mk_arrow(Ul, nat, nat, Ul)))
env = add_decl(env, mk_var_decl("dlst", {l1, l2}, mk_arrow(mk_sort(l1), mk_sort(l2), mk_sort(max_univ(l1, l2)))))
env = add_decl(env, mk_var_decl("vec2lst", {l}, Pi(A, n, mk_arrow(vec_l(A, n), lst_l(A)))))
env = add_decl(env, mk_var_decl("lst2vec", {l}, Pi(A, ll, vec_l(A, len_l(A, ll)))))
env = add_decl(env, mk_var_decl("vec2mat", {l}, Pi(A, n, mk_arrow(vec_l(A, n), mat_l(A, n, one)))))
env = add_decl(env, mk_var_decl("mat2dlst", {l}, Pi(A, n, m, mk_arrow(mat_l(A, n, m), Const("dlst", {l, 1})(A, nat)))))
env = add_decl(env, mk_var_decl("nat2lst", mk_arrow(nat, lst_1(nat))))
env = add_coercion(env, "lst2vec")
assert(is_coercion(env, Const("lst2vec", {l})))
assert(has_coercions_from(env, "lst"))
local lst_nat = lst_1(nat)
print(get_coercion(env, lst_nat, "vec"))
env = add_coercion(env, "vec2mat")
print(get_coercion(env, lst_nat, "mat"))
assert(env:type_check(get_coercion(env, lst_nat, "mat")))
for_each_coercion_user(env, function(C, D, f) print(tostring(C) .. " >-> " .. tostring(D) .. " : " .. tostring(f)) end)
env = add_coercion(env, "nat2lst")
print("---------")
for_each_coercion_user(env, function(C, D, f) print(tostring(C) .. " >-> " .. tostring(D) .. " : " .. tostring(f)) end)
print(get_coercion(env, nat, "mat"))
assert(env:type_check(get_coercion(env, nat, "mat")))
env = add_coercion(env, "mat2dlst")
print("---------")
for_each_coercion_user(env, function(C, D, f) print(tostring(C) .. " >-> " .. tostring(D) .. " : " .. tostring(f)) end)
print(get_coercion(env, lst_nat, "dlst"))
assert(env:type_check(get_coercion(env, lst_nat, "dlst")))

env:export("coe1_mod.olean")
local env2 = import_modules("coe1_mod")
print(get_coercion(env2, lst_nat, "dlst"))
assert(env2:type_check(get_coercion(env2, lst_nat, "dlst")))
assert(is_coercion(env2, "vec2mat"))
assert(is_coercion(env2, "lst2vec"))
env2 = add_decl(env2, mk_var_decl("lst2vec2", {l}, Pi(A, ll, vec_l(A, len_l(A, ll)))))
print("======")
env2 = add_coercion(env2, "lst2vec2")
print("======")
print(get_coercion(env2, lst_nat, "dlst"))
print("---------")
for_each_coercion_user(env2, function(C, D, f) print(tostring(C) .. " >-> " .. tostring(D) .. " : " .. tostring(f)) end)
env2 = add_coercion(env2, "vec2lst")
env2 = add_decl(env2, mk_var_decl("lst2nat", {l}, Pi(A, mk_arrow(lst_l(A), nat))))
env2 = add_coercion(env2, "lst2nat")
print("---------")
for_each_coercion_user(env2, function(C, D, f) print(tostring(C) .. " >-> " .. tostring(D)) end)
for_each_coercion_user(env2, function(C, D, f) print(tostring(C) .. " >-> " .. tostring(D) .. " : " .. tostring(f)) end)

assert(has_coercions_from(env2, lst_nat))
assert(not has_coercions_from(env2, Const("foo")))
assert(not has_coercions_from(env2, Const("lst", {1})))
assert(has_coercions_from(env2, Const("vec", {1})(nat, one)))
assert(not has_coercions_from(env2, Const("vec", {1})(nat)))
assert(not has_coercions_from(env2, Const("vec")(nat, one)))

print("Coercions (vec nat one): ")
cs = get_user_coercions(env2, Const("vec", {1})(nat, one))
for i = 1, #cs do
   print(tostring(cs[i][1]) .. " : " .. tostring(cs[i][3]) .. " : " .. tostring(cs[i][2]))
end
