#include "Diff_funcs.h"

Node* Diff_new_node(int node_type, const void* node_value, size_t node_val_size, Node* left_node, Node* right_node) {

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

Node* Diff_take_derivative(Node* root) {

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

        #define EL_FUNC_SIGN(func_name, sign) case func_name:\
                                                  return Diff_##func_name(root);\

        #define EL_FUNC(func_name)            case func_name:\
                                                  return Diff_##func_name(root);\

            #include "Code_gen_funcs.h"
            #undef EL_FUNC_SIGN
            #undef EL_FUNC

            case LOG:
                return Diff_LOG(root);

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

    if(root->left_node)
        Diff_dtor(root->left_node);

    if(root->right_node)
        Diff_dtor(root->right_node);

    Node_data* tmp_node_data_ptr = NULL;
    memcpy(&tmp_node_data_ptr, &root->node_data, sizeof(Node_data*));

    free(tmp_node_data_ptr);
    free(root);
}
