#ifndef TEX_OUTPUT
#define TEX_OUTPUT

#include "Diff_funcs.h"

#define TEX_PRINTF(...) fprintf(output_file, __VA_ARGS__)

const uint64_t Max_depth = 3;
const uint64_t Min_depth = 4;

void TeX_introduction(FILE* output_file);
void TeX_start_diff(FILE* output_file);
void TeX_output_expr(Node* root, FILE* output_file, bool Do_replacements, uint64_t* replacements_count);
void TeX_print_expr(Node* root, FILE* output_file, Stack_t* stack, Node** root_copy,
                    int64_t* global_depth, uint64_t* replacements_count, bool Do_replacements);
bool Get_tree_depth(Node* root);
bool Count_tree_depth(Node* root, uint64_t* curr_depth, uint64_t* max_depth);
bool System_call_TeX(const char* output_file_name);
#endif
