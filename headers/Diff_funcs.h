#ifndef DIFF_FUNCS
#define DIFF_FUNCS

#include <math.h>
#include "Diff_definitions.h"
#include "..\Binary_Trees\Tree_funcs.h"
#include "Elementary_diff_funcs.h"

const size_t Mem_blocks_amount = 1;

struct Buffer_data {

    char* buffer;
    int64_t buffer_size;
};

struct Variable_data {

    int64_t var_name;
    double var_temp_value;
};

struct Var_list {

    Variable_data* var_list;
    size_t var_list_size;
    size_t free_var;
};

const size_t Var_list_init_size = 10;
const size_t Var_list_expansion_coeff = 2;
const double Var_list_init_value = 0;
//const double Trivial_number = M_PI; //for checking trvial cases;

Node* Diff_new_node(int node_type, const void* node_value, size_t node_val_size, Node* left_node, Node* right_node);
void Diff_dtor(Node* root);

Node* Diff_take_derivative(Node* root);
bool Is_subtree_constant(Node* root);
Node* Copy_node(Node* root);

#define _NUM(number) Diff_new_node(NUM, &number, sizeof(double), NULL, NULL)

#define _VAR(variable) Diff_new_node(VAR, &variable, sizeof(int64_t), NULL, NULL)
#define _X Diff_new_node(VAR, &X_var, sizeof(int64_t), NULL, NULL)

#define _ADD(left_node, right_node) Diff_new_node(OP, &ADD, sizeof(int64_t), left_node, right_node)
#define _SUB(left_node, right_node) Diff_new_node(OP, &SUB, sizeof(int64_t), left_node, right_node)
#define _MUL(left_node, right_node) Diff_new_node(OP, &MUL, sizeof(int64_t), left_node, right_node)
#define _DIV(left_node, right_node) Diff_new_node(OP, &DIV, sizeof(int64_t), left_node, right_node)
#define _POW(left_node, right_node) Diff_new_node(OP, &POW, sizeof(int64_t), left_node, right_node)
#define _LOG(left_node, right_node) Diff_new_node(OP, &LOG, sizeof(int64_t), left_node, right_node)

#define _LN(left_node)   Diff_new_node(OP, &LN, sizeof(int64_t), left_node, NULL)
#define _SH(left_node)   Diff_new_node(OP, &SH, sizeof(int64_t), left_node, NULL)
#define _CH(left_node)   Diff_new_node(OP, &CH, sizeof(int64_t), left_node, NULL)
#define _TH(left_node)   Diff_new_node(OP, &TH, sizeof(int64_t), left_node, NULL)

#define _SIN(left_node)  Diff_new_node(OP, &SIN, sizeof(int64_t), left_node, NULL)
#define _COS(left_node)  Diff_new_node(OP, &COS, sizeof(int64_t), left_node, NULL)
#define _TAN(left_node)  Diff_new_node(OP, &TAN, sizeof(int64_t), left_node, NULL)
#define _COT(left_node)  Diff_new_node(OP, &COT, sizeof(int64_t), left_node, NULL)
#define _EXP(left_node)  Diff_new_node(OP, &EXP, sizeof(int64_t), left_node, NULL)
#define _CTH(left_node)  Diff_new_node(OP, &CTH, sizeof(int64_t), left_node, NULL)
#define _ASH(left_node)  Diff_new_node(OP, &ASH, sizeof(int64_t), left_node, NULL)
#define _ACH(left_node)  Diff_new_node(OP, &ACH, sizeof(int64_t), left_node, NULL)
#define _ATH(left_node)  Diff_new_node(OP, &ATH, sizeof(int64_t), left_node, NULL)

#define _ASIN(left_node) Diff_new_node(OP, &ASIN, sizeof(int64_t), left_node, NULL)
#define _ACOS(left_node) Diff_new_node(OP, &ACOS, sizeof(int64_t), left_node, NULL)
#define _ATAN(left_node) Diff_new_node(OP, &ATAN, sizeof(int64_t), left_node, NULL)
#define _ACOT(left_node) Diff_new_node(OP, &ACOT, sizeof(int64_t), left_node, NULL)
#define _ACTH(left_node) Diff_new_node(OP, &ACTH, sizeof(int64_t), left_node, NULL)

#include "Diff_read_funcs.h"
#include "Var_list_funcs.h"
#include "Evaluation_funcs.h"
#include "Simplification_funcs.h"

#endif
