#ifndef REPL_LIST
#define REPL_LIST

#include "Diff_funcs.h"

const int64_t Rep_list_error_value = -1;
const size_t Rep_list_init_size = 2;
const size_t Rep_list_expansion_coeff = 2;
const double Rep_list_init_value = 0;

bool Rep_list_ctor(Rep_list* rep_list, size_t init_size);
int64_t Insert_replacement(Rep_list* rep_list, Node* root);
bool Rep_list_expand(Rep_list* rep_list);

#endif
