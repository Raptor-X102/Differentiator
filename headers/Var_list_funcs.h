#ifndef VAR_LIST
#define VAR_LIST

#include "Diff_funcs.h"

bool Var_list_ctor(Var_list* var_list, size_t init_size);
int64_t Find_variable(Var_list* var_list, int64_t variable);
bool Insert_var(Var_list* var_list, int64_t variable, double var_value);
bool Var_list_expand(Var_list* var_list);

#endif
