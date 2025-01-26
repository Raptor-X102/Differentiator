#ifndef DIFF_FUNCS
#define DIFF_FUNCS

#include <math.h>
#include "Diff_definitions.h"
#include "..\Binary_Trees\Tree_funcs.h"
#include "..\Binary_Trees\Binary_tree_graph_debug.h"
#include "Elementary_diff_funcs.h"

const size_t Mem_blocks_amount = 1;
const int Power_buffer_size = 12; // INT_MAX digits = 10
const int Decimal_number_system = 10;

struct Buffer_data {

    char* buffer;
    int64_t buffer_size;
};

struct Variable_data {

    char* var_name;
    uint64_t var_len;
    double var_value;
};

struct Var_list {

    Variable_data* var_list;
    size_t var_list_size;
    size_t free_var;
};

struct Rep_list {

    Node** nodes_array;
    size_t rep_list_size;
    size_t free_pos;
};

const int Error_value = -1;

Node* Diff_new_node(int node_type, const void* node_value, size_t node_val_size,
                    Node* left_node, Node* right_node);
void Diff_dtor(Node* root);

Node* Diff_take_derivative(Node* root, FILE* output_file, uint64_t* replacements_count,
                           bool write_explanation);
bool Is_subtree_constant(Node* root);
Node* Copy_node(Node* root);

Node* Differentiate_expression(Node* root, char** write_files,
                               void (*Print_node_data) (const void* data, FILE* file));
Node* Taylor_output(Node* root, Var_list* var_list, double x0, double x,
                    int order, FILE* output_file);
Node* Taylor_extension(Node* root, Var_list* var_list, double x0, double x,
                      int order, FILE* output_file, uint64_t* replacements);

#define _NUM(number) Diff_new_node(NUM, &number, sizeof(double), NULL, NULL)

#define _VAR(variable) Diff_new_node(VAR, &variable, sizeof(Variable_data*), NULL, NULL)
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
#define _ABS(left_node)  Diff_new_node(OP, &ABS, sizeof(int64_t), left_node, NULL)

#define _ASIN(left_node) Diff_new_node(OP, &ASIN, sizeof(int64_t), left_node, NULL)
#define _ACOS(left_node) Diff_new_node(OP, &ACOS, sizeof(int64_t), left_node, NULL)
#define _ATAN(left_node) Diff_new_node(OP, &ATAN, sizeof(int64_t), left_node, NULL)
#define _ACOT(left_node) Diff_new_node(OP, &ACOT, sizeof(int64_t), left_node, NULL)
#define _ACTH(left_node) Diff_new_node(OP, &ACTH, sizeof(int64_t), left_node, NULL)
#define _SQRT(left_node) Diff_new_node(OP, &SQRT, sizeof(int64_t), left_node, NULL)

#include "Var_list_funcs.h"
#include "Diff_read_funcs.h"
#include "Evaluation_funcs.h"
#include "Simplification_funcs.h"
#include "TeX_output.h"

#endif
