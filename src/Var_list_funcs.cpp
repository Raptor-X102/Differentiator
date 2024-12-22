#include "Var_list_funcs.h"

bool Var_list_ctor(Var_list* var_list, size_t init_size) {

    Variable_data* tmp_ptr = (Variable_data*) calloc(init_size, sizeof(Variable_data));

    if(!tmp_ptr) {

        DEBUG_PRINTF("ERROR: memory was not allocated\n\n");
        return false;
    }

    var_list->var_list = tmp_ptr;
    var_list->var_list_size = init_size;

    return true;
}

int64_t Find_variable(Var_list* var_list, int64_t variable) {

    for(int64_t index = 0; index < var_list->var_list_size; index++) {

        if(var_list->var_list[index].var_name == variable)
            return index;
    }

    return -1;
}

bool Insert_var(Var_list* var_list, int64_t variable, double var_value) {

    int64_t index = Find_variable(var_list, variable);
    if(index >= 0)
        var_list->var_list[index].var_temp_value = var_value;

    else {

        var_list->var_list[var_list->free_var].var_temp_value = var_value;
        var_list->var_list[var_list->free_var].var_name = variable;
        var_list->free_var++;
    }

    if(var_list->free_var >= var_list->var_list_size)
        if(!Var_list_expand(var_list))
            return false;

    return true;
}

bool Var_list_expand(Var_list* var_list) {

    Variable_data* tmp_ptr = (Variable_data*) realloc(var_list->var_list,
                                          var_list->var_list_size *
                                          sizeof(Variable_data)   *
                                          Var_list_expansion_coeff);

    if(!tmp_ptr) {

        DEBUG_PRINTF("ERROR: memory was not allocated\n\n");
        return false;
    }

    var_list->var_list = tmp_ptr;
    return true;
}
