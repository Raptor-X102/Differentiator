#ifndef DIFF_READ_FUNCS
#define DIFF_READ_FUNCS

#include "Diff_funcs.h"

struct Lexeme_data {

    Node* root;
    int64_t line;
    int64_t col;
};


Node* Diff_read_expression(const char* filename, Var_list* var_list);
Buffer_data Diff_read_file(const char* filename);
Node* Diff_create_tree_full_brackets(const char* expr_buffer, uint64_t data_size,
                                     Var_list* var_list, int64_t* curr_pos);

Lexeme_data* Lexeme_separator(char* expr_buffer, int64_t expr_buffer_size);
Node* Get_Grammar(Lexeme_data* lexeme_array, int64_t* curr_lex);
Node* Get_addition_priority(Lexeme_data* lexeme_array, int64_t* curr_lex);
Node* Get_multiplication_priority(Lexeme_data* lexeme_array, int64_t* curr_lex);
Node* Get_power_priority(Lexeme_data* lexeme_array, int64_t* curr_lex);
Node* Get_first_priority(Lexeme_data* lexeme_array, int64_t* curr_lex);
Node* Get_number(Lexeme_data* lexeme_array, int64_t* curr_lex);
Node* Get_function(Lexeme_data* lexeme_array, int64_t* curr_lex);
void Syntax_error(Lexeme_data* lexeme_array, int64_t* curr_lex);

void Diff_get_next_symbol(const char* node_data, uint64_t data_size, int64_t* curr_pos);
Node_data Find_node_data(const char* expr_buffer, uint64_t data_size, int64_t* curr_pos);

#endif
