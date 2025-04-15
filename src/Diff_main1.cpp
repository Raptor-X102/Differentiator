#include "Diff_funcs.h"
#include "Diff_debug_funcs.h"
#include "Check_r_w_flags.h"

int main(int argc, char* argv[]) {

    char* read_files[1] = {};
    char* write_files[3] = {};
    file_types files = {};
    files.read_files = read_files;
    files.write_files = write_files;
    if(!Check_r_w_flags(CHECK_R_W, argv, argc, &files)) {

        DEBUG_PRINTF("ERROR: flags verification failed")
        return 0;
    }

    Buffer_data expression_buffer = {};
    //double d1 = 3, d2 = 1000, d3 = 7;
    /*Var_list var_list = {};

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
    var_list.var_list[index].var_value = 2;

    Node* root_diff = Diff_take_derivative(root);
    if(!root_diff) {

        DEBUG_PRINTF("ERROR: function returned NULL (diff)\n");
        return 0;
    }

    DEBUG_PRINTF("%s\n", files.write_files[0]);

    Simplify_expression(&root_diff);*/

    Var_list var_list_variables = {};

    if(!Var_list_ctor(&var_list_variables, Var_list_init_size)) {

        DEBUG_PRINTF("ERROR: var_list null pointer\n");
        return false;
    }

    Node* root = Diff_read_expression(files.read_files[0], &var_list_variables, &expression_buffer);
    if(!root) {

        DEBUG_PRINTF("ERROR: function returned NULL\n");
        return false;
    }

    Node* root_diff = Differentiate_expression(root, files.write_files,
                                               &Print_node_data);

    FILE* output_file = fopen(files.write_files[2], "ab");
    if(!output_file) {

        DEBUG_PRINTF("ERROR: file was not opened\n");
        return 0;
    }

    double x0 = 0, x = 0.3;
    int order = 4;
    Node* Taylor_formula = Taylor_output(root, &var_list_variables, x0, x, order, output_file);

    Tree_graph_debug(Taylor_formula, files.write_files[1], Print_node_data);
    free(Taylor_formula);

    TEX_PRINTF("\\end{document}");

    fclose(output_file);
    System_call_TeX(files.write_files[2]);
    free(expression_buffer.buffer);
    Diff_dtor(root);
    Diff_dtor(root_diff);
    Var_list_dtor(&var_list_variables);
}
