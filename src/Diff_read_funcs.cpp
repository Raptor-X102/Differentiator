#include "Diff_read_funcs.h"

Node* Diff_read_expression(const char* filename, Var_list* var_list) {

    Buffer_data buffer_data = Diff_read_file(filename);
    if(!buffer_data.buffer || buffer_data.buffer_size < 0)
        return NULL;
    DEBUG_PRINTF("\nbuffer_data.buffer_size = %d\n\n", buffer_data.buffer_size);
    //int64_t curr_pos = 0;

    //Node* root = Diff_create_tree_full_brackets(buffer_data.buffer, buffer_data.buffer_size, var_list, &curr_pos);
    Lexeme_data* lexeme_arr = Lexeme_separator(buffer_data.buffer, buffer_data.buffer_size);

    Node_data* tmp_data = NULL;

    for(int i = 0; i < 6; i++) {

        memcpy(&tmp_data, &lexeme_arr[i].root->node_data, sizeof(Node_data*));
        DEBUG_PRINTF("%x ", tmp_data->expression_type);
        DEBUG_PRINTF("%x", tmp_data->value);
        DEBUG_PRINTF("(%c)\n", tmp_data->value);
    }
    getchar();

    DEBUG_PRINTF("SEPARATOR ENDED\n");
    int64_t curr_lex = 0;
    Node* root = Get_Grammar(lexeme_arr, &curr_lex);
    free(buffer_data.buffer);
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


Node* Diff_create_tree_full_brackets(const char* expr_buffer, uint64_t data_size, Var_list* var_list, int64_t* curr_pos) {

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

    if(tmp_data.expression_type == VAR) { //TODO: var_list

        int64_t index = Find_variable(var_list, tmp_data.value);
        if(index < 0)
            Insert_var(var_list, tmp_data.value, Var_list_init_value);

        return _VAR(tmp_data.value);
    }

    if(tmp_data.expression_type == OP) {

        if((tmp_data.value & is_binary) == is_binary) {

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
}

Lexeme_data* Lexeme_separator(char* expr_buffer, int64_t expr_buffer_size) {

    DEBUG_PRINTF("SEPARATOR STARTED\n");
    Lexeme_data* lexeme_array = (Lexeme_data*) calloc(expr_buffer_size, sizeof(Lexeme_data));
    size_t lexeme_array_pos = 0;

    if(!lexeme_array) {

        DEBUG_PRINTF("ERROR: memory was not allocated\n");
        return NULL;
    }

    for(int64_t curr_pos = 0, line = 0, col = 1; curr_pos < expr_buffer_size; col++) {

        DEBUG_PRINTF("(%lld) %c\n", curr_pos, expr_buffer[curr_pos]);

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

        #define EL_FUNC_SIGN(func_name, sign) else if(expr_buffer[curr_pos] == sign) {\
                                                \
                                                    lexeme_array[lexeme_array_pos].root = _##func_name(NULL, NULL);\
                                                    curr_pos++;\
                                                    lexeme_array_pos++;\
                                                }

        /*else if(!_strnicmp(expr_buffer + curr_pos, "sin")) {

                                        lexeme_array[lexeme_array_pos].root = _SIN(NULL);
                                        while(isalpha(expr_buffer[curr_pos]))
                                            curr_pos++;

                                        lexeme_array_pos++;
                                    }*/

        #define EL_FUNC(func_name) else if(!_strnicmp(expr_buffer + curr_pos, #func_name, strlen(#func_name))) {\
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
            lexeme_array[lexeme_array_pos].root = _VAR(expr_buffer[curr_pos]);
            while(isalpha(expr_buffer[curr_pos]))
                curr_pos++;

            lexeme_array_pos++;
        }
    }

    return lexeme_array;
}

Node* Get_Grammar(Lexeme_data* lexeme_array, int64_t* curr_lex) {

    DEBUG_PRINTF("GRAMMAR STARTED\n");
    Node* root = Get_addition_priority(lexeme_array, curr_lex);

    Node_data* tmp_data = NULL;
    DEBUG_PRINTF("lexeme_array[*curr_lex].root = %p\n", lexeme_array[*curr_lex].root);
    DEBUG_PRINTF("lexeme_array[*curr_lex].root->node_data = %p\n", lexeme_array[*curr_lex].root->node_data);
    DEBUG_PRINTF("%d\n", *curr_lex);
    memcpy(&tmp_data, &lexeme_array[*curr_lex].root->node_data, sizeof(Node_data*));

    if(tmp_data->value != EOT || tmp_data->expression_type != SPECIAL_SYMBOL) {

        Syntax_error(lexeme_array, curr_lex);
        return NULL;
    }

    DEBUG_PRINTF("GRAMMAR ENDED\n");
    return root;
}

Node* Get_addition_priority(Lexeme_data* lexeme_array, int64_t* curr_lex) {

    Node* root = Get_multiplication_priority(lexeme_array, curr_lex);

    Node_data* tmp_data = NULL;
    DEBUG_PRINTF("%d\n", *curr_lex);
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
        DEBUG_PRINTF("WHILE %d\n", *curr_lex);
    }

    DEBUG_PRINTF("RETURNED\n");
    return root;
}

Node* Get_multiplication_priority(Lexeme_data* lexeme_array, int64_t* curr_lex) {

    Node* root = Get_power_priority(lexeme_array, curr_lex);

    Node_data* tmp_data = NULL;
    DEBUG_PRINTF("%d\n", *curr_lex);
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
    DEBUG_PRINTF("%d\n", *curr_lex);
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
    DEBUG_PRINTF("%d\n", *curr_lex);
    DEBUG_PRINTF("%d\n", *curr_lex);
    memcpy(&tmp_data, &lexeme_array[*curr_lex].root->node_data, sizeof(Node_data*));

    if(tmp_data->value == BRACKET_OP && tmp_data->expression_type == SPECIAL_SYMBOL) {

        (*curr_lex)++;
        Node* root = Get_addition_priority(lexeme_array, curr_lex);

        DEBUG_PRINTF("%d\n", *curr_lex);
        memcpy(&tmp_data, &lexeme_array[*curr_lex].root->node_data, sizeof(Node_data*));
        if(tmp_data->value != BRACKET_CL || tmp_data->expression_type != SPECIAL_SYMBOL)
            Syntax_error(lexeme_array, curr_lex);

        (*curr_lex)++;
        return root;
    }

    else if(tmp_data->expression_type == NUM){      // Get_Num

        DEBUG_PRINTF("NUM\n");
        (*curr_lex)++;
        return lexeme_array[*curr_lex - 1].root;
    }

    else if(tmp_data->expression_type == OP) { // Get_Func

        return Get_function(lexeme_array, curr_lex);
    }

    else if(tmp_data->expression_type == VAR) { // Get_Id

        DEBUG_PRINTF("VAR\n");
        (*curr_lex)++;
        return lexeme_array[*curr_lex - 1].root;
    }

    else
        return NULL;
}

Node* Get_number(Lexeme_data* lexeme_array, int64_t* curr_lex) {

    Node_data* tmp_data = NULL;
    DEBUG_PRINTF("%d\n", *curr_lex);
    memcpy(&tmp_data, &lexeme_array[*curr_lex].root->node_data, sizeof(Node_data*));
    (*curr_lex)++;
    return _NUM(tmp_data->value);
}

Node* Get_function(Lexeme_data* lexeme_array, int64_t* curr_lex) {

    Node_data* tmp_data = NULL;
    DEBUG_PRINTF("%d\n", *curr_lex);
    memcpy(&tmp_data, &lexeme_array[*curr_lex].root->node_data, sizeof(Node_data*));

    if(tmp_data->value == LOG) {

        int64_t func_index = (*curr_lex);

        (*curr_lex)++;
        DEBUG_PRINTF("lexeme_array[*curr_lex].root = %p\n", lexeme_array[*curr_lex].root);
        DEBUG_PRINTF("lexeme_array[*curr_lex].root->node_data = %p\n", lexeme_array[*curr_lex].root->node_data);
        DEBUG_PRINTF("%d\n", *curr_lex);
        memcpy(&tmp_data, &lexeme_array[*curr_lex].root->node_data, sizeof(Node_data*));

        if(tmp_data->value != BRACKET_OP || tmp_data->expression_type != SPECIAL_SYMBOL)
            Syntax_error(lexeme_array, curr_lex);

        (*curr_lex)++;
        Node* left_node = Get_addition_priority(lexeme_array, curr_lex);

        if(!left_node)
            Syntax_error(lexeme_array, curr_lex);

        DEBUG_PRINTF("%d\n", *curr_lex);
        memcpy(&tmp_data, &lexeme_array[*curr_lex].root->node_data, sizeof(Node_data*));

        if(tmp_data->value != COMMA || tmp_data->expression_type != SPECIAL_SYMBOL)
            Syntax_error(lexeme_array, curr_lex);

        (*curr_lex)++;
        Node* right_node = Get_addition_priority(lexeme_array, curr_lex);

        if(!right_node)
            Syntax_error(lexeme_array, curr_lex);

        DEBUG_PRINTF("%d\n", *curr_lex);
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
        DEBUG_PRINTF("%d\n", (*curr_lex));
        DEBUG_PRINTF("lexeme_array[*curr_lex].root = %p\n", lexeme_array[*curr_lex].root);
        DEBUG_PRINTF("lexeme_array[*curr_lex].root->node_data = %p\n", lexeme_array[*curr_lex].root->node_data);
        DEBUG_PRINTF("%d\n", *curr_lex);
        memcpy(&tmp_data, &lexeme_array[*curr_lex].root->node_data, sizeof(Node_data*));

        if(tmp_data->value != BRACKET_OP || tmp_data->expression_type != SPECIAL_SYMBOL)
            Syntax_error(lexeme_array, curr_lex);

        (*curr_lex)++;
        Node* left_node = Get_addition_priority(lexeme_array, curr_lex);

        if(!left_node)
            Syntax_error(lexeme_array, curr_lex);

        DEBUG_PRINTF("%d\n", *curr_lex);
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
    DEBUG_PRINTF("%d\n", *curr_lex);
    memcpy(&tmp_data, &lexeme_array[*curr_lex].root->node_data, sizeof(Node_data*));

    DEBUG_PRINTF("Syntax Error:\n"
                 "line: %lld, col: %lld\n"
                 "lexeme: expression_type = %lld, value = %lld", lexeme_array[*curr_lex].line,
                                                                 lexeme_array[*curr_lex].col,
                                                                 tmp_data->expression_type,
                                                                 tmp_data->value);
    abort();
}

void Diff_get_next_symbol(const char* node_data, uint64_t data_size, int64_t* curr_pos) {

    while(isspace(node_data[*curr_pos]) && *curr_pos < data_size)
        (*curr_pos)++;

    DEBUG_PRINTF("(%d) = ", *curr_pos);
    DEBUG_PRINTF("%c ", node_data[*curr_pos]);
    DEBUG_PRINTF("%d\n", node_data[*curr_pos]);
}

Node_data Find_node_data(const char* expr_buffer, uint64_t data_size, int64_t* curr_pos) {

    Node_data tmp_data = {};

    if(isdigit(expr_buffer[*curr_pos]) || expr_buffer[*curr_pos] == '-' && isdigit(expr_buffer[*curr_pos+1])) {

        double tmp_double = atof(expr_buffer+*curr_pos);
        memcpy(&tmp_data.value, &tmp_double, sizeof(double));
        tmp_data.expression_type = NUM;
        for(; expr_buffer[*curr_pos] != ')' && *curr_pos < data_size; (*curr_pos)++);
        return tmp_data;
    }

    #define EL_FUNC(func_name) else if(!_stricmp(expr_buffer + *curr_pos, #func_name)) {\
                                \
                                    tmp_data.expression_type = OP;\
                                    tmp_data.value = func_name;\
                                    (*curr_pos) += strlen(#func_name);\
                                }

    #define EL_FUNC_SIGN(func_name, sign) else if(expr_buffer[*curr_pos] == sign) {\
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
}
