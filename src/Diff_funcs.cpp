#include "Diff_funcs.h"

Node* Differentiate_expression(Node* root, char** write_files,
                               void (*Print_node_data) (const void* data, FILE* file)) {

    FILE* output_file = fopen(write_files[2], "wb+");
    if(!output_file) {

        DEBUG_PRINTF("ERROR: output file was not opened\n");
        return NULL;
    }

    uint64_t replacements_count = 0;

    TeX_introduction(output_file);
    TeX_start_diff(output_file);
    TEX_PRINTF("\\[");
    TeX_output_expr(root, output_file, true, &replacements_count);
    TEX_PRINTF("\n");

    Node* root_diff = Diff_take_derivative(root, output_file, &replacements_count, true);
    if(!root_diff) {

        DEBUG_PRINTF("ERROR: function returned NULL (diff)\n");
        return NULL;
    }

    Simplify_expression(&root_diff);
    TEX_PRINTF("\n Result: \n\\[");
    TeX_output_expr(root_diff, output_file, true, &replacements_count);

    TEX_PRINTF("\n");

    Tree_graph_debug(root, write_files[0], Print_node_data);
    Tree_graph_debug(root_diff, write_files[1], Print_node_data);

    fclose(output_file);
    return root_diff;
}

Node* Diff_new_node(int node_type, const void* node_value, size_t node_val_size,
                    Node* left_node, Node* right_node) {

    Node_data* node_data_ptr = (Node_data*) calloc(Mem_blocks_amount, sizeof(Node_data));
    if(!node_data_ptr) {

        DEBUG_PRINTF("ERROR: memory was not allocated\n");
        return NULL;
    }

    node_data_ptr->expression_type = node_type;
    memcpy(&node_data_ptr->value, node_value, node_val_size);

    Node* node = NULL;
    if(!Create_new_node(&node, &node_data_ptr, sizeof(Node_data*)))
        return NULL;

    node->left_node = left_node;
    node->right_node = right_node;

    return node;
}

Node* Diff_take_derivative(Node* root, FILE* output_file, uint64_t* replacements_count,
                           bool write_explanation) {

    Node_data* tmp_node_data = NULL;
    memcpy(&tmp_node_data, &root->node_data, sizeof(Node_data*));

    if(tmp_node_data->expression_type == VAR) {

        double tmp_double = 1;
        return _NUM(tmp_double);
    }

    if(tmp_node_data->expression_type == NUM) {

        double tmp_double = 0;
        return _NUM(tmp_double);
    }

    if(tmp_node_data->expression_type == OP) {

        switch(tmp_node_data->value) {

        #define EL_FUNC_SIGN(func_name, sign, TeX_sign) case func_name:\
                                                  return Diff_##func_name(root, output_file, \
                                                         replacements_count, write_explanation);

        #define EL_FUNC(func_name, func_full_name)            case func_name:\
                                                  return Diff_##func_name(root, output_file, \
                                                         replacements_count, write_explanation);

            #include "Code_gen_funcs.h"
            #undef EL_FUNC_SIGN
            #undef EL_FUNC

            case LOG:
                return Diff_LOG(root, output_file,
                                replacements_count, write_explanation);

            default:
                DEBUG_PRINTF("ERROR: wrong func format\n");
                return NULL;
        }
    }
}

bool Is_subtree_constant(Node* root) {

    bool no_var_left = true, no_var_right = true;

    Node_data* tmp_node_data = NULL;
    memcpy(&tmp_node_data, &root->node_data, sizeof(Node_data*));

    if(tmp_node_data->expression_type == VAR)
        return false;

    if(root->left_node)
        no_var_left = Is_subtree_constant(root->left_node);

    if(root->right_node)
        no_var_right = Is_subtree_constant(root->right_node);

    if(!no_var_left || !no_var_right)
        return false;

    else
        return true;
}

Node* Copy_node(Node* root) {

    Node* tmp_node_ptr = (Node*) calloc(Mem_blocks_amount, sizeof(Node));
    Node_data* tmp_node_data_ptr = (Node_data*) calloc(Mem_blocks_amount, sizeof(Node_data));
    Node_data* root_data = NULL;

    if(!tmp_node_ptr || !tmp_node_data_ptr) {

        DEBUG_PRINTF("ERROR: memory was not allocated\n");
        return NULL;
    }

    memcpy(&root_data, &root->node_data, sizeof(Node_data*));
    tmp_node_data_ptr->expression_type = root_data->expression_type;
    memcpy(&tmp_node_data_ptr->value, &root_data->value, sizeof(int64_t));
    memcpy(&tmp_node_ptr->node_data, &tmp_node_data_ptr, sizeof(Node_data*));

    Node* left_node = NULL, * right_node = NULL;

    if(root->left_node)
        left_node = Copy_node(root->left_node);

    if(root->right_node)
        right_node = Copy_node(root->right_node);

    tmp_node_ptr->left_node = left_node;
    tmp_node_ptr->right_node = right_node;

    return tmp_node_ptr;
}

void Diff_dtor(Node* root) {

    if(root) {

        if(root->left_node)
            Diff_dtor(root->left_node);

        if(root->right_node)
            Diff_dtor(root->right_node);

        Node_data* tmp_node_data_ptr = NULL;
        memcpy(&tmp_node_data_ptr, &root->node_data, sizeof(Node_data*));

        free(tmp_node_data_ptr);
        free(root);
    }

}

Node* Taylor_output(Node* root, Var_list* var_list, double x0, double x,
                    int order, FILE* output_file) {

    TEX_PRINTF("\nTaylor extension: \n");
    uint64_t replacements = 0;
    TEX_PRINTF("\\[ f(%lg) = ", x);
    Node* Taylor_formula = Taylor_extension(root, var_list, x0, x,
                                            order, output_file, &replacements);
    DEBUG_PRINTF("Taylor_formula = %p\n", Taylor_formula);
    char power_buffer[Power_buffer_size] = {};

    //************Wanted to print remainder term in Peano form****/
    /*
    TEX_PRINTF(" + o(");

    if(order > 0) {

        TEX_PRINTF("(x_0-x)^");
        itoa(order, power_buffer, Decimal_number_system);
        fputs(power_buffer, output_file);
    }
    else
        TEX_PRINTF("1");

    TEX_PRINTF(")");
    */
    double Taylor_result = Evaluate_tree(var_list, Taylor_formula);
    TEX_PRINTF(" \\stackrel{}{\\approx} %lg\\]\n", Taylor_result);

    return Taylor_formula;
}

Node* Taylor_extension(Node* root, Var_list* var_list, double x0, double x,
                      int order, FILE* output_file, uint64_t* replacements) {

    for(int i = 0; i < var_list->var_list_size; i++)
        var_list->var_list[i].var_value = x0;

    Node* derivative = Copy_node(root), * derivative_copy = NULL;
    double result = Evaluate_tree(var_list, derivative);
    Node* evaluated = _NUM(result);
    Node* Taylor_formula = evaluated;
    TeX_output_expr(Taylor_formula, output_file, false, replacements);

    if(order > 0) {

        double difference = x - x0;
        Node* difference_node = _SUB(_NUM(x), _NUM(x0));
        derivative = Diff_take_derivative(derivative, output_file, replacements, false);
        Simplify_expression(&derivative);
        derivative_copy = derivative;
        result = Evaluate_tree(var_list, derivative);
        evaluated = _MUL(_NUM(result), difference_node);
        TEX_PRINTF(" + ");
        TeX_output_expr(evaluated, output_file, false, replacements);
        Taylor_formula = _ADD(Taylor_formula, evaluated);

        for(int i = 2; i <= order; i++) {

            double factorial = (double) Fact(i);
            DEBUG_PRINTF("factorial = %lg\n", factorial);

            Node* fact_node = _NUM(factorial);
            difference_node = _SUB(_NUM(x), _NUM(x0));
            double double_i = (double) i;
            DEBUG_PRINTF("double_i = %lg\n", double_i);

            Node* difference_power = _POW(difference_node, _NUM(double_i));
            derivative = Diff_take_derivative(derivative, output_file, replacements, false);

            Simplify_expression(&derivative);
            free(derivative_copy);

            derivative_copy = derivative;
            result = Evaluate_tree(var_list, derivative);
            evaluated = _DIV(_NUM(result), fact_node);

            TEX_PRINTF(" + ");
            TeX_output_expr(evaluated, output_file, false, replacements);
            TEX_PRINTF("(");
            TeX_output_expr(difference_node, output_file, false, replacements);
            TEX_PRINTF(")^%d", i);

            evaluated = _MUL(evaluated, difference_power);
            Taylor_formula = _ADD(Taylor_formula, evaluated);
        }

        free(derivative_copy);
    }

    return Taylor_formula;
}
