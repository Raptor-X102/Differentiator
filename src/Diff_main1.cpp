#include <TXLib.h>
#include "Diff_funcs.h"
#include "Diff_debug_funcs.h"
#include "..\Check_flags\Check_r_w_flags.h"

void Print_node_data(const void* data, FILE* file);

int main(int argc, char* argv[]) {

    char* read_files[1] = {};
    char* write_files[2] = {};
    file_types files = {};
    files.read_files = read_files;
    files.write_files = write_files;
    if(!Check_r_w_flags(CHECK_R_W, argv, argc, &files)) {

        DEBUG_PRINTF("ERROR: flags verification failed")
        return 0;
    }

    //double d1 = 3, d2 = 1000, d3 = 7;
    Var_list var_list = {};

    if(!Var_list_ctor(&var_list, Var_list_init_size)) {

        DEBUG_PRINTF("ERROR: var_list null pointer\n");
        return NULL;
    }

    Node* root = Diff_read_expression(files.read_files[0], &var_list);
    if(!root) {

        DEBUG_PRINTF("ERROR: function returned NULL\n");
        return 0;
    }

    int64_t index = Find_variable(&var_list, X_var);
    var_list.var_list[index].var_temp_value = 2;

    Node* root_diff = Diff_take_derivative(root);
    if(!root_diff) {

        DEBUG_PRINTF("ERROR: function returned NULL (diff)\n");
        return 0;
    }

    DEBUG_PRINTF("%s\n", files.write_files[0]);

    Simplify_expression(&root_diff);



    Tree_graph_debug(root, files.write_files[0], Print_node_data);
    Tree_graph_debug(root_diff, files.write_files[1], Print_node_data);

    /*double expr_val = Evaluate_tree(&var_list, root);
    double diif_val = Evaluate_tree(&var_list, root_diff);

    printf("expr_val = %lg\n", expr_val);
    printf("root_diff = %lg\n", diif_val);*/

    /*Node* evaluated_node = Evaluate_const_node(root);
    Node_data* tmp_root_data = NULL;
    if(evaluated_node) {

        memcpy(&tmp_root_data, &evaluated_node->node_data, sizeof(Node_data*));
        DEBUG_PRINTF("tmp_root_data->expression_type = %d\n", tmp_root_data->expression_type);
        double temp_d = 1;
        memcpy(&temp_d, &tmp_root_data->value, sizeof(double));
        DEBUG_PRINTF("temp_d = %lg\n", temp_d);
    }

    else
        DEBUG_PRINTF("ERROR: function returned NULL (eval)\n");*/

    Diff_dtor(root);
    Diff_dtor(root_diff);

}

void Print_node_data(const void* data, FILE* file) {

    DEBUG_PRINTF("\nPrinting\n");
    DEBUG_PRINTF("data = %p\n", data);

    Node_data* tmp_node_data = NULL;
    memcpy(&tmp_node_data, data, sizeof(Node_data*));
    DEBUG_PRINTF("tmp_node_data = %p\n", tmp_node_data);
    DEBUG_PRINTF("tmp_node_data->expression_type = %d\n", tmp_node_data->expression_type);
    DEBUG_PRINTF("tmp_node_data->value = %d\n", tmp_node_data->value);
    switch(tmp_node_data->expression_type) {
        case NUM: {

            fprintf(file, "expression_type = %d, \\n", NUM);
            double tmp_double = 0;
            memcpy(&tmp_double, &tmp_node_data->value, sizeof(double));
            fprintf(file, "value = %lg", tmp_double);
            break;
        }

        case VAR: {

            fprintf(file, "expression_type = %d, \\n", VAR);
            int64_t tmp_int = 0;
            memcpy(&tmp_int, &tmp_node_data->value, sizeof(int64_t));
            fprintf(file, "value = %c", (char) tmp_int);
            break;
        }

        case OP: {

            fprintf(file, "expression_type = %d, \\n", OP);
            int64_t tmp_int = 0;
            memcpy(&tmp_int, &tmp_node_data->value, sizeof(int64_t));
            switch(tmp_int) {

                case ADD:
                    tmp_int = '+';
                    fprintf(file, "value = %c", (char) tmp_int);
                    break;

                case SUB:
                    tmp_int = '-';
                    fprintf(file, "value = %c", (char) tmp_int);
                    break;

                case MUL:
                    tmp_int = '*';
                    fprintf(file, "value = %c", (char) tmp_int);
                    break;

                case DIV:
                    tmp_int = '/';
                    fprintf(file, "value = %c", (char) tmp_int);
                    break;

                case LN:
                    fprintf(file, "value = ln");
                    break;

                case LOG:
                    fprintf(file, "value = log");
                    break;

                case SIN:
                    fprintf(file, "value = sin");
                    break;

                case COS:
                    fprintf(file, "value = cos");
                    break;

                case TAN:
                    fprintf(file, "value = tan");
                    break;

                case COT:
                    fprintf(file, "value = cot");
                    break;

                case POW:
                    fprintf(file, "value = ^");
                    break;

                case EXP:
                fprintf(file, "value = exp");
                break;

                default:
                    DEBUG_PRINTF("ERROR: wrong expression type \n");
            };

            if(!tmp_int)
                DEBUG_PRINTF("ERROR: wrong expression type (0 operation)\n");

            break;

        }

        default:
            DEBUG_PRINTF("ERROR: wrong expression type\n");
    }
}
