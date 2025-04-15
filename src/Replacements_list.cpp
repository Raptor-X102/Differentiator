#include "Replacements_list.h"

bool Rep_list_ctor(Rep_list* rep_list, size_t init_size) {

    Node** tmp_ptr = (Node**) calloc(init_size, sizeof(Node*));

    if(!tmp_ptr) {

        DEBUG_PRINTF("ERROR: memory was not allocated\n\n");
        return false;
    }

    rep_list->nodes_array = tmp_ptr;
    rep_list->rep_list_size = init_size;

    return true;
}

int64_t Insert_replacement(Rep_list* rep_list, Node* root) {

    int64_t index = rep_list->free_pos;
    rep_list->nodes_array[rep_list->free_pos] = root;
    rep_list->free_pos++;

    if(rep_list->free_pos >= rep_list->rep_list_size)
        if(!Rep_list_expand(rep_list))
            return Var_list_error_value;

    return index;
}

bool Rep_list_expand(Rep_list* rep_list) {

    Node** tmp_ptr = (Node**) realloc(rep_list->nodes_array,
                                          rep_list->rep_list_size *
                                          sizeof(Node*)           *
                                          Var_list_expansion_coeff);

    if(!tmp_ptr) {

        DEBUG_PRINTF("ERROR: memory was not allocated\n\n");
        return false;
    }

    rep_list->nodes_array = tmp_ptr;
    return true;
}
