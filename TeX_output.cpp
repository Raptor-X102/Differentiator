#include "TeX_output.h"

void TeX_introduction(FILE* output_file) {

    TEX_PRINTF("\\documentclass{article}\n");
    TEX_PRINTF("\\usepackage{amsmath}\n\\DeclareMathOperator{\\arctg}{arctg}\n");
    TEX_PRINTF("\\begin{document}\n");
}

void TeX_output_expr(const Node* root, FILE* output_file) {

    Node_data* node_data = NULL;
    memcpy(&node_data, &root->node_data, sizeof(Node_data*));

    switch(node_data->expression_type) {

        case OP: {

            if(node_data->value == LOG || node_data->value == DIV) { // prefix case

                if(node_data->value == LOG)
                    TEX_PRINTF("\\log_{");

                if(node_data->value == DIV)
                    TEX_PRINTF("\\frac{");

                if(root->left_node)
                    TeX_output_expr(root->left_node, output_file);

                TEX_PRINTF("} {");

                if(root->right_node) {

                    Node_data* node_data_c = node_data;
                    memcpy(&node_data, &root->right_node->node_data, sizeof(Node_data*));
                    if(node_data_c->value == LOG && node_data->expression_type == OP &&
                      (node_data->value == ADD || node_data->value == SUB)) {

                        TEX_PRINTF("(");
                        TeX_output_expr(root->right_node, output_file);
                        TEX_PRINTF(")");
                    }

                    else
                        TeX_output_expr(root->right_node, output_file);
                }

                TEX_PRINTF("}");
            }

            else if(node_data->value & Is_binary){ // infix case

                if(node_data->value == POW) {

                    TEX_PRINTF("{");
                    if(root->left_node) {

                        memcpy(&node_data, &root->left_node->node_data, sizeof(Node_data*));
                        if(node_data->expression_type == OP &&
                           (node_data->value != POW)) {

                            TEX_PRINTF("(");
                            TeX_output_expr(root->left_node, output_file);
                            TEX_PRINTF(")");
                        }

                        else
                            TeX_output_expr(root->left_node, output_file);
                    }

                    TEX_PRINTF("}^{");

                    if(root->right_node) {

                        memcpy(&node_data, &root->right_node->node_data, sizeof(Node_data*));
                        if(node_data->expression_type == OP &&
                        (node_data->value == ADD || node_data->value == SUB)) {

                            TEX_PRINTF("(");
                            TeX_output_expr(root->right_node, output_file);
                            TEX_PRINTF(")");
                        }

                        else
                            TeX_output_expr(root->right_node, output_file);
                    }

                    TEX_PRINTF("}");
                }

                else {

                    if(root->left_node) {


                        TEX_PRINTF("{");
                        TeX_output_expr(root->left_node, output_file);
                        TEX_PRINTF("}");
                    }

                    switch(node_data->value) {

                        #define EL_FUNC_SIGN(func_name, sign, TeX_sign) case func_name: {\
                                                                            fputs(TeX_sign, output_file);\
                                                                            break;\
                                                                        }

                        #define EL_FUNC(func_name, func_full_name)

                        #include "Code_gen_funcs.h"
                        #undef EL_FUNC_SIGN
                        #undef EL_FUNC

                        default:
                            DEBUG_PRINTF("ERROR: BINARY OPERATION WAS NOT FOUND");
                    }

                    if(root->right_node) {

                        TEX_PRINTF("{");
                        TeX_output_expr(root->right_node, output_file);
                        TEX_PRINTF("}");
                    }
                }
            }

            else {

                switch(node_data->value) {

                    #define EL_FUNC_SIGN(func_name, sign, TeX_sign)

                    #define EL_FUNC(func_name, func_full_name) case func_name: {\
                                                                    fputs(func_full_name, output_file);\
                                                                    break;\
                                                                }

                    #include "Code_gen_funcs.h"
                    #undef EL_FUNC_SIGN
                    #undef EL_FUNC

                    default:
                        DEBUG_PRINTF("node_data->value = %#x\n", node_data->value);
                        DEBUG_PRINTF("ERROR: UNARY OPERATION WAS NOT FOUND\n");
                }

                if(root->left_node) {

                    TEX_PRINTF("{");
                    memcpy(&node_data, &root->left_node->node_data, sizeof(Node_data*));
                    if(node_data->expression_type == OP && (node_data->value == ADD ||
                                                            node_data->value == SUB ||
                                                            node_data->value == MUL)) {

                        TEX_PRINTF("(");
                        TeX_output_expr(root->left_node, output_file);
                        TEX_PRINTF(")");
                    }

                    else
                        TeX_output_expr(root->left_node, output_file);

                    TEX_PRINTF("}");
                }
            }

            break;
        }

        case NUM: {

            double tmp_double = 0;
            memcpy(&tmp_double, &node_data->value, sizeof(double));
            TEX_PRINTF("%.4lg", tmp_double);
            break;
        }

        case VAR: {

            int64_t tmp_int = 0;
            memcpy(&tmp_int, &node_data->value, sizeof(int64_t));
            TEX_PRINTF("%c", (char) tmp_int);
            break;
        }

        default:
            DEBUG_PRINTF("ERROR: unknown expression type\n");
    }
}
