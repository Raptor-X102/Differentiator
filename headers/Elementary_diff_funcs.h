#ifndef ELEM_DIFF_FUNCS
#define ELEM_DIFF_FUNCS

#include "Diff_funcs.h"

#define EL_FUNC_SIGN(func_name, sign, TeX_sign) Node* Diff_##func_name(Node* root, \
                                                                       FILE* output_file, \
                                                                       uint64_t* replacements_count, \
                                                                       bool write_explanation);
#define EL_FUNC(func_name, func_full_name) Node* Diff_##func_name(Node* root, \
                                                                  FILE* output_file, \
                                                                  uint64_t* replacements_count, \
                                                                  bool write_explanation);

Node* Diff_LOG(Node* root, FILE* output_file, uint64_t* replacements_count, bool write_explanation);

#include "Code_gen_funcs.h"
#undef EL_FUNC_SIGN
#undef EL_FUNC

#endif
