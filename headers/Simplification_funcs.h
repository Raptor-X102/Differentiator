#ifndef SIMPLIFICATION_FUNCS
#define SIMPLIFICATION_FUNCS

#include "Diff_funcs.h"

bool Simplify_expression(Node** root);
bool Simplify_expression_left(Node** root, int64_t operation, double left_constant);
bool Simplify_expression_right(Node** root, int64_t operation, double right_constant);

#endif
