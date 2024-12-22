#ifndef ELEM_DIFF_FUNCS
#define ELEM_DIFF_FUNCS

#include "Diff_funcs.h"

#define EL_FUNC_SIGN(func_name, sign) Node* Diff_##func_name(Node* root);
#define EL_FUNC(func_name) Node* Diff_##func_name(Node* root);

Node* Diff_LOG(Node* root);

#include "Code_gen_funcs.h"
#undef EL_FUNC_SIGN
#undef EL_FUNC

#endif
