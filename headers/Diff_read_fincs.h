#ifndef DIFF_READ_FUNCS
#define DIFF_READ_FUNCS

Node* Diff_read_expression(const char* filename, Var_list* var_list);
Buffer_data Diff_read_file(const char* filename);
Node* Diff_create_tree_full_brackets(const char* expr_buffer, uint64_t data_size,
                                     Var_list* var_list, int64_t* curr_pos);
void Diff_get_next_symbol(const char* node_data, uint64_t data_size, int64_t* curr_pos);

#endif
