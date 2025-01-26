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

bool Var_list_dtor(Var_list* var_list) {

    if(var_list && var_list->var_list) {

        free(var_list->var_list);
        return true;
    }

    else
        return false;
}

int64_t Find_variable(Var_list* var_list, char* variable, uint64_t var_len) {

    for(int64_t index = 0; index < var_list->free_var; index++) {

        if(!strncmp(var_list->var_list[index].var_name, variable, var_len))
            return index;
    }

    return -1;
}

int64_t Insert_var(Var_list* var_list, char* variable, double var_value, uint64_t var_len) {

    int64_t index = Find_variable(var_list, variable, var_len);
    if(index >= 0)
        var_list->var_list[index].var_value = var_value;

    else {

        var_list->var_list[var_list->free_var].var_value = var_value;
        var_list->var_list[var_list->free_var].var_name = variable;
        var_list->var_list[var_list->free_var].var_len = var_len;
        index = var_list->free_var;
        var_list->free_var++;

        if(var_list->free_var >= var_list->var_list_size)
            if(!Var_list_expand(var_list))
                return Var_list_error_value;
    }

    return index;
}

bool Var_list_expand(Var_list* var_list) {

    var_list->var_list_size = var_list->var_list_size*
                              Var_list_expansion_coeff;

    Variable_data* tmp_ptr = (Variable_data*) realloc(var_list->var_list,
                                                      var_list->var_list_size *
                                                      sizeof(Variable_data));

    if(!tmp_ptr) {

        DEBUG_PRINTF("ERROR: memory was not allocated\n\n");
        return false;
    }

    var_list->var_list = tmp_ptr;
    return true;
}
