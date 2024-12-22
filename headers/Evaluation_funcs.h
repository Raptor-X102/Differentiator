#ifndef EVAL_FUNCS
#define EVAL_FUNCS

#include "Diff_funcs.h"

Node* Evaluate_const_node(Node* root);
double Evaluate_tree(Var_list* var_list, Node* root);
double Do_arithmetics(int64_t operation, double num1, double num2);

#endif
