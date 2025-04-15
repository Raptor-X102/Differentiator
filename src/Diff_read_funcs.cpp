#include "Diff_read_funcs.h"

Node* Diff_read_expression(const char* filename, Var_list* var_list, Buffer_data* expression_buffer) {

    *expression_buffer = Diff_read_file(filename);
    if(!expression_buffer->buffer || expression_buffer->buffer_size < 0)
        return NULL;

    Lexeme_data* lexeme_arr = Lexeme_separator(expression_buffer->buffer, expression_buffer->buffer_size, var_list);

    /*for(int i = 0; i < 6; i++) {

        memcpy(&tmp_data, &lexeme_arr[i].root->node_data, sizeof(Node_data*));
        DEBUG_PRINTF("%x ", tmp_data->expression_type);
        DEBUG_PRINTF("%x", tmp_data->value);
        DEBUG_PRINTF("(%c)\n", tmp_data->value);
    }
    getchar();*/

    int64_t curr_lex = 0;
    Node* root = Get_Grammar(lexeme_arr, &curr_lex);
    return root;
}

Buffer_data Diff_read_file(const char* filename) {

    Buffer_data buffer_data = {};
    int64_t file_size = get_file_size(filename);

    if(file_size < 0)
        return buffer_data;

    buffer_data.buffer_size = file_size;

    FILE* input_file = fopen(filename, "rb");
    if(!input_file) {

        DEBUG_PRINTF("ERROR: file was not opened\n");
        return buffer_data;
    }

    char* file_buffer = (char*) calloc(file_size, sizeof(char));
    if(!file_buffer) {

        DEBUG_PRINTF("ERROR: memory was not allocated\n");
        return buffer_data;
    }

    fread(file_buffer, sizeof(char), file_size, input_file);
    if(ferror(input_file)) {

        DEBUG_PRINTF("ERROR: fread failed\n");
        return buffer_data;
    }

    buffer_data.buffer = file_buffer;

    return buffer_data;
}

//***************First version of reading data***************/

/*Node* Diff_create_tree_full_brackets(const char* expr_buffer, uint64_t data_size, Var_list* var_list, int64_t* curr_pos) {

    Diff_get_next_symbol(expr_buffer, data_size, curr_pos);

    Node* left_node = NULL, * right_node = NULL;
    if(expr_buffer[*curr_pos] == '(') {

        (*curr_pos)++;
        left_node = Diff_create_tree_full_brackets(expr_buffer, data_size, var_list, curr_pos);
        Diff_get_next_symbol(expr_buffer, data_size, curr_pos);
        if(expr_buffer[*curr_pos] == ')') {

            (*curr_pos)++;
            return left_node;
        }
    }

    Node_data tmp_data = Find_node_data(expr_buffer, data_size, curr_pos);
    DEBUG_PRINTF("tmp_data.expression_type = %d\n", tmp_data.expression_type);
    DEBUG_PRINTF("tmp_data.value = %d\n", tmp_data.value);
    Diff_get_next_symbol(expr_buffer, data_size, curr_pos);

    if(tmp_data.expression_type == NUM) {

        return _NUM(tmp_data.value);
    }

    if(tmp_data.expression_type == VAR) {

        int64_t index = Find_variable(var_list, tmp_data.value);
        if(index < 0)
            Insert_var(var_list, tmp_data.value, Var_list_init_value);

        return _VAR(tmp_data.value);
    }

    if(tmp_data.expression_type == OP) {

        if((tmp_data.value & Is_binary) == Is_binary) {

            DEBUG_PRINTF("\nIS BINARY\n");
            //(*curr_pos)++;
            right_node = Diff_create_tree_full_brackets(expr_buffer, data_size, var_list, curr_pos);
            Diff_get_next_symbol(expr_buffer, data_size, curr_pos);
            DEBUG_PRINTF("\n\n SUS: (%d)", *curr_pos);
            DEBUG_PRINTF(" %c\n", expr_buffer[*curr_pos]);
            (*curr_pos)++;
            DEBUG_PRINTF("(%d)", *curr_pos);
            DEBUG_PRINTF(" %c\n", expr_buffer[*curr_pos]);
            return Diff_new_node(OP, &tmp_data.value, sizeof(int64_t), left_node, right_node);
        }

        else {

            if(tmp_data.value == LOG) {

                left_node = Diff_create_tree_full_brackets(expr_buffer, data_size, var_list, curr_pos);
                (*curr_pos)++;
                Diff_get_next_symbol(expr_buffer, data_size, curr_pos);
                if(expr_buffer[*curr_pos] == ',') {

                    (*curr_pos)++;
                    Diff_get_next_symbol(expr_buffer, data_size, curr_pos);
                }

                right_node = Diff_create_tree_full_brackets(expr_buffer, data_size, var_list, curr_pos);
                (*curr_pos)++;
                return Diff_new_node(OP, &tmp_data.value, sizeof(int64_t), left_node, right_node);
            }

            else {

                left_node = Diff_create_tree_full_brackets(expr_buffer, data_size, var_list, curr_pos);
                return Diff_new_node(OP, &tmp_data.value, sizeof(int64_t), left_node, NULL);
            }
        }
    }

    else {

        DEBUG_PRINTF("ERROR: no coincidences\n");
        return NULL;
    }
}*/

Lexeme_data* Lexeme_separator(char* expr_buffer, int64_t expr_buffer_size, Var_list* var_list) {

    Lexeme_data* lexeme_array = (Lexeme_data*) calloc(expr_buffer_size, sizeof(Lexeme_data));
    size_t lexeme_array_pos = 0;

    if(!lexeme_array) {

        DEBUG_PRINTF("ERROR: memory was not allocated\n");
        return NULL;
    }

    for(int64_t curr_pos = 0, line = 1, col = 1; curr_pos < expr_buffer_size; col++) {

        if(isspace(expr_buffer[curr_pos])) {
            if(expr_buffer[curr_pos] == '\n') {

                line++;
                col = 0;
            }

            curr_pos++;
            continue;
        }

        lexeme_array[lexeme_array_pos].line = line;
        lexeme_array[lexeme_array_pos].col = col;

        if(expr_buffer[curr_pos] == '$') {

            lexeme_array[lexeme_array_pos].root = Diff_new_node(SPECIAL_SYMBOL, &EOT,
                                                                sizeof(int64_t),
                                                                NULL, NULL);
            curr_pos++;
            lexeme_array_pos++;
            //return lexeme_array;
        }

        else if(expr_buffer[curr_pos] == ',') {

            lexeme_array[lexeme_array_pos].root = Diff_new_node(SPECIAL_SYMBOL, &COMMA,
                                                                sizeof(int64_t),
                                                                NULL, NULL);
            curr_pos++;
            lexeme_array_pos++;
        }

        else if (expr_buffer[curr_pos] == '(' ||
                expr_buffer[curr_pos] == ')') {

            if(expr_buffer[curr_pos] == '(')
                lexeme_array[lexeme_array_pos].root = Diff_new_node(SPECIAL_SYMBOL, &BRACKET_OP,
                                                                    sizeof(int64_t),
                                                                    NULL, NULL);

            else
                lexeme_array[lexeme_array_pos].root = Diff_new_node(SPECIAL_SYMBOL, &BRACKET_CL,
                                                                    sizeof(int64_t),
                                                                    NULL, NULL);

            lexeme_array_pos++;
            curr_pos++;
        }

        else if(isdigit(expr_buffer[curr_pos]) ||
                expr_buffer[curr_pos] == '-' && isdigit(expr_buffer[curr_pos+1])) {

            double tmp_double = 0;
            int num_len = 0;
            sscanf(expr_buffer + curr_pos, "%lf%n", &tmp_double, &num_len);
            curr_pos += num_len;
            lexeme_array[lexeme_array_pos].root = _NUM(tmp_double);
            lexeme_array_pos++;
        }

        else if(!_strnicmp(expr_buffer + curr_pos, "log", strlen("log"))) {

            lexeme_array[lexeme_array_pos].root = _LOG(NULL, NULL);
            while(isalpha(expr_buffer[curr_pos]))
                curr_pos++;

            lexeme_array_pos++;
        }

        #define EL_FUNC_SIGN(func_name, sign, TeX_sign) else if(expr_buffer[curr_pos] == sign) {\
                                                \
                                                    lexeme_array[lexeme_array_pos].root = _##func_name(NULL, NULL);\
                                                    curr_pos++;\
                                                    lexeme_array_pos++;\
                                                }

        #define EL_FUNC(func_name, func_full_name) else if(!_strnicmp(expr_buffer + curr_pos, #func_name, strlen(#func_name)) ||\
                                                           !_strnicmp(expr_buffer + curr_pos, func_full_name + 1, strlen(func_full_name)-1)) {\
                                    \
                                        lexeme_array[lexeme_array_pos].root = _##func_name(NULL);\
                                        while(isalpha(expr_buffer[curr_pos]))\
                                            curr_pos++;\
                                    \
                                        lexeme_array_pos++;\
                                    }

        #include "Code_gen_funcs.h"
        #undef EL_FUNC_SIGN
        #undef EL_FUNC

        else {

            if(expr_buffer[curr_pos] == 'e' && !isalpha(expr_buffer[curr_pos + 1])) {

                double tmp_double = M_E;
                curr_pos++;
                lexeme_array[lexeme_array_pos].root = _NUM(tmp_double);
                lexeme_array_pos++;
            }

            else if(!strncmp(expr_buffer + curr_pos, "pi", 2) && !isalpha(expr_buffer[curr_pos + 2])) {

                double tmp_double = M_PI;
                curr_pos += 2;
                lexeme_array[lexeme_array_pos].root = _NUM(tmp_double);
                lexeme_array_pos++;
            }

            else {

                uint64_t pos_copy = curr_pos;
                while(isalpha(expr_buffer[curr_pos]))
                    curr_pos++;

                uint64_t var_len = curr_pos - pos_copy;

                int64_t index = Insert_var(var_list, expr_buffer + pos_copy, Var_list_init_value, var_len);
                if(index == Var_list_error_value) {

                    DEBUG_PRINTF("ERROR: insertion failed\n");
                    return NULL;
                }

                Variable_data* vl_ptr = var_list->var_list + index;

                lexeme_array[lexeme_array_pos].root = _VAR(vl_ptr);

                lexeme_array_pos++;
            }
        }
    }

    return lexeme_array;
}

Node* Get_Grammar(Lexeme_data* lexeme_array, int64_t* curr_lex) {

    Node* root = Get_addition_priority(lexeme_array, curr_lex);

    Node_data* tmp_data = NULL;
    memcpy(&tmp_data, &lexeme_array[*curr_lex].root->node_data, sizeof(Node_data*));

    if(tmp_data->value != EOT || tmp_data->expression_type != SPECIAL_SYMBOL) {

        Syntax_error(lexeme_array, curr_lex);
        return NULL;
    }

    return root;
}

Node* Get_addition_priority(Lexeme_data* lexeme_array, int64_t* curr_lex) {

    Node* root = Get_multiplication_priority(lexeme_array, curr_lex);

    Node_data* tmp_data = NULL;
    memcpy(&tmp_data, &lexeme_array[*curr_lex].root->node_data, sizeof(Node_data*));

    while(tmp_data->expression_type == OP &&
         (tmp_data->value == ADD || tmp_data->value == SUB)) {

        Node* operation = lexeme_array[*curr_lex].root;
        operation->left_node = root;
        (*curr_lex)++;
        Node* right_node = Get_multiplication_priority(lexeme_array, curr_lex);
        memcpy(&tmp_data, &lexeme_array[*curr_lex].root->node_data, sizeof(Node_data*));
        operation->right_node = right_node;

        root = operation;
    }

    return root;
}

Node* Get_multiplication_priority(Lexeme_data* lexeme_array, int64_t* curr_lex) {

    Node* root = Get_power_priority(lexeme_array, curr_lex);

    Node_data* tmp_data = NULL;
    memcpy(&tmp_data, &lexeme_array[*curr_lex].root->node_data, sizeof(Node_data*));

    while(tmp_data->expression_type == OP &&
         (tmp_data->value == MUL || tmp_data->value == DIV)) {

        Node* operation = lexeme_array[*curr_lex].root;
        operation->left_node = root;
        (*curr_lex)++;
        Node* right_node = Get_power_priority(lexeme_array, curr_lex);
        memcpy(&tmp_data, &lexeme_array[*curr_lex].root->node_data, sizeof(Node_data*));
        operation->right_node = right_node;

        root = operation;
    }

    return root;
}

Node* Get_power_priority(Lexeme_data* lexeme_array, int64_t* curr_lex) {

    Node* root = Get_first_priority(lexeme_array, curr_lex);

    Node_data* tmp_data = NULL;
    memcpy(&tmp_data, &lexeme_array[*curr_lex].root->node_data, sizeof(Node_data*));

    while(tmp_data->expression_type == OP && tmp_data->value == POW) {

        Node* operation = lexeme_array[*curr_lex].root;
        operation->left_node = root;
        (*curr_lex)++;
        Node* right_node = Get_first_priority(lexeme_array, curr_lex);
        memcpy(&tmp_data, &lexeme_array[*curr_lex].root->node_data, sizeof(Node_data*));
        operation->right_node = right_node;

        root = operation;
    }

    return root;
}

Node* Get_first_priority(Lexeme_data* lexeme_array, int64_t* curr_lex) {

    Node_data* tmp_data = NULL;
    memcpy(&tmp_data, &lexeme_array[*curr_lex].root->node_data, sizeof(Node_data*));

    if(tmp_data->value == BRACKET_OP && tmp_data->expression_type == SPECIAL_SYMBOL) {

        (*curr_lex)++;
        Node* root = Get_addition_priority(lexeme_array, curr_lex);
        memcpy(&tmp_data, &lexeme_array[*curr_lex].root->node_data, sizeof(Node_data*));
        if(tmp_data->value != BRACKET_CL || tmp_data->expression_type != SPECIAL_SYMBOL)
            Syntax_error(lexeme_array, curr_lex);

        (*curr_lex)++;
        return root;
    }

    else if(tmp_data->expression_type == NUM){

        (*curr_lex)++;
        return lexeme_array[*curr_lex - 1].root;
    }

    else if(tmp_data->expression_type == OP) {

        return Get_function(lexeme_array, curr_lex);
    }

    else if(tmp_data->expression_type == VAR) {

        (*curr_lex)++;
        return lexeme_array[*curr_lex - 1].root;
    }

    else
        return NULL;
}

Node* Get_number(Lexeme_data* lexeme_array, int64_t* curr_lex) {

    Node_data* tmp_data = NULL;
    memcpy(&tmp_data, &lexeme_array[*curr_lex].root->node_data, sizeof(Node_data*));
    (*curr_lex)++;
    return _NUM(tmp_data->value);
}

Node* Get_function(Lexeme_data* lexeme_array, int64_t* curr_lex) {

    Node_data* tmp_data = NULL;
    memcpy(&tmp_data, &lexeme_array[*curr_lex].root->node_data, sizeof(Node_data*));

    if(tmp_data->value == LOG) {

        int64_t func_index = (*curr_lex);

        (*curr_lex)++;
        memcpy(&tmp_data, &lexeme_array[*curr_lex].root->node_data, sizeof(Node_data*));

        if(tmp_data->value != BRACKET_OP || tmp_data->expression_type != SPECIAL_SYMBOL)
            Syntax_error(lexeme_array, curr_lex);

        (*curr_lex)++;
        Node* left_node = Get_addition_priority(lexeme_array, curr_lex);

        if(!left_node)
            Syntax_error(lexeme_array, curr_lex);

        memcpy(&tmp_data, &lexeme_array[*curr_lex].root->node_data, sizeof(Node_data*));

        if(tmp_data->value != COMMA || tmp_data->expression_type != SPECIAL_SYMBOL)
            Syntax_error(lexeme_array, curr_lex);

        (*curr_lex)++;
        Node* right_node = Get_addition_priority(lexeme_array, curr_lex);

        if(!right_node)
            Syntax_error(lexeme_array, curr_lex);

        memcpy(&tmp_data, &lexeme_array[*curr_lex].root->node_data, sizeof(Node_data*));

        if(tmp_data->value != BRACKET_CL || tmp_data->expression_type != SPECIAL_SYMBOL)
            Syntax_error(lexeme_array, curr_lex);

        (*curr_lex)++;

        lexeme_array[func_index].root->left_node = left_node;
        lexeme_array[func_index].root->right_node = right_node;

        return lexeme_array[func_index].root;
    }

    else {

        int64_t func_index = (*curr_lex);
        (*curr_lex)++;
        memcpy(&tmp_data, &lexeme_array[*curr_lex].root->node_data, sizeof(Node_data*));

        if(tmp_data->value != BRACKET_OP || tmp_data->expression_type != SPECIAL_SYMBOL)
            Syntax_error(lexeme_array, curr_lex);

        (*curr_lex)++;
        Node* left_node = Get_addition_priority(lexeme_array, curr_lex);

        if(!left_node)
            Syntax_error(lexeme_array, curr_lex);

        memcpy(&tmp_data, &lexeme_array[*curr_lex].root->node_data, sizeof(Node_data*));

        if(tmp_data->value != BRACKET_CL || tmp_data->expression_type != SPECIAL_SYMBOL)
            Syntax_error(lexeme_array, curr_lex);

        (*curr_lex)++;

        lexeme_array[func_index].root->left_node = left_node;

        return lexeme_array[func_index].root;
    }
}

void Syntax_error(Lexeme_data* lexeme_array, int64_t* curr_lex) {

    Node_data* tmp_data = NULL;
    memcpy(&tmp_data, &lexeme_array[*curr_lex].root->node_data, sizeof(Node_data*));

    switch (tmp_data->expression_type) {

        case VAR: {

            Variable_data* var_data = NULL;
            memcpy(&var_data, &tmp_data->value, sizeof(Variable_data*));
            DEBUG_PRINTF("value = %.*s\n", (size_t) var_data->var_len, var_data->var_name);
            break;
        }

        case NUM: {

            double tmp_double = 0;
            memcpy(&tmp_double, &tmp_data->value, sizeof(double));
            DEBUG_PRINTF("value = %lg\n", tmp_double);
            break;
        }

        case OP: {

            int64_t tmp_int = 0;
            memcpy(&tmp_int, &tmp_data->value, sizeof(int64_t));
            DEBUG_PRINTF("value = %d\n", tmp_int);
            break;
        }

        default:
            DEBUG_PRINTF("ERROR: unknown expression type\n");
    }

    abort();
}

void Diff_get_next_symbol(const char* node_data, uint64_t data_size, int64_t* curr_pos) {

    while(isspace(node_data[*curr_pos]) && *curr_pos < data_size)
        (*curr_pos)++;
}

//***************for the first version of reading data***************/

/*Node_data Find_node_data(const char* expr_buffer, uint64_t data_size, int64_t* curr_pos) {

    Node_data tmp_data = {};

    if(isdigit(expr_buffer[*curr_pos]) || expr_buffer[*curr_pos] == '-' && isdigit(expr_buffer[*curr_pos+1])) {

        double tmp_double = atof(expr_buffer+*curr_pos);
        memcpy(&tmp_data.value, &tmp_double, sizeof(double));
        tmp_data.expression_type = NUM;
        for(; expr_buffer[*curr_pos] != ')' && *curr_pos < data_size; (*curr_pos)++);
        return tmp_data;
    }

    #define EL_FUNC(func_name, func_full_name) else if(!_stricmp(expr_buffer + *curr_pos, #func_name)) {\
                                \
                                    tmp_data.expression_type = OP;\
                                    tmp_data.value = func_name;\
                                    (*curr_pos) += strlen(#func_name);\
                                }

    #define EL_FUNC_SIGN(func_name, sign, TeX_sign) else if(expr_buffer[*curr_pos] == sign) {\
                                \
                                    tmp_data.expression_type = OP;\
                                    tmp_data.value = func_name;\
                                    (*curr_pos)++;\
                                }

    if(!_stricmp(expr_buffer + *curr_pos, "log")) {

        tmp_data.expression_type = OP;
        tmp_data.value = LOG;
        (*curr_pos) += 4;
    }

    #include "Code_gen_funcs.h"
    #undef EL_FUNC_SIGN
    #undef EL_FUNC

    else {

        tmp_data.expression_type = VAR;
        tmp_data.value = expr_buffer[*curr_pos];
        for(; expr_buffer[*curr_pos] != ')' && *curr_pos < data_size; (*curr_pos)++);
    }

    return tmp_data;
}*/
