#include "Simplification_funcs.h"

bool Simplify_expression(Node** root) {

/*Trivial cases:

    left or right const:
    1. Multiply 0
    2. Multiply 1
    5. Add 0
    6. Sub 0

    left is const:
    7. 0 - expr (mul -1 instead)
    9. 0 ^ expr

    right:
    3. Divide by 1
    4. expr ^ 1
    10. expr ^ 0

*/
    Node_data* tmp_root_data = NULL, * tmp_node_data1 = NULL, * tmp_node_data2 = NULL;
    if((*root))
        memcpy(&tmp_root_data, &(*root)->node_data, sizeof(Node_data*));

    if(tmp_root_data->expression_type == NUM)
        return true;


    if(tmp_root_data->expression_type == VAR)
        return false;


    if(tmp_root_data->expression_type == OP) {

        bool is_left_const = true, is_right_const = true;
        double tmp_double_left = 0, tmp_double_right = 0;

        if((*root)->left_node) {

            memcpy(&tmp_node_data1, &(*root)->left_node->node_data, sizeof(Node_data*));
            is_left_const = Simplify_expression(&(*root)->left_node);

            if (is_left_const) {

                memcpy(&tmp_node_data1, &(*root)->left_node->node_data, sizeof(Node_data*));
                memcpy(&tmp_double_left, &tmp_node_data1->value, sizeof(double));
            }
        }

        if((*root)->right_node) {

            memcpy(&tmp_node_data2, &(*root)->right_node->node_data, sizeof(Node_data*));
            is_right_const = Simplify_expression(&(*root)->right_node);

            if (is_right_const) {

                memcpy(&tmp_node_data2, &(*root)->right_node->node_data, sizeof(Node_data*));
                memcpy(&tmp_double_right, &tmp_node_data2->value, sizeof(double));
            }
        }

        if(is_left_const && is_right_const) {

            Node* new_node = Evaluate_const_node(*root);
            Tree_Dtor(*root);
            *root = new_node;
            Node* evaluated_node = new_node;
            Node_data* tmp_data = NULL;
            if(evaluated_node) {

                memcpy(&tmp_data, &evaluated_node->node_data, sizeof(Node_data*));
                double temp_d = 1;
                memcpy(&temp_d, &tmp_data->value, sizeof(double));
            }

            else
                DEBUG_PRINTF("ERROR: function returned NULL (?)\n");

            return true;
        }

        else if (is_left_const)
            return Simplify_expression_left(root, tmp_root_data->value, tmp_double_left);

        else if(is_right_const)
            return Simplify_expression_right(root, tmp_root_data->value, tmp_double_right);

        else
            return false;
    }

    return false;
}

bool Simplify_expression_left(Node** root, int64_t operation, double left_constant) {

    if(fabs(left_constant) < Epsilon && (operation == MUL || operation == POW)) {

        double tmp_d_zero = 0;
        Tree_Dtor(*root);
        *root = _NUM(tmp_d_zero);
        return true;
    }

    else if(fabs(left_constant - 1) < Epsilon && operation == POW) {

        double tmp_d = 1;
        Tree_Dtor(*root);
        *root = _NUM(tmp_d);
        return true;
    }

    else if(fabs(left_constant - 1) < Epsilon && operation == MUL ||
            fabs(left_constant)     < Epsilon && operation == ADD) {

        Node* new_node = Copy_node((*root)->right_node);
        Tree_Dtor(*root);
        *root = new_node;
        return false;
    }

    else if(fabs(left_constant) < Epsilon && operation == SUB) {

        double tmp_d_neg_1 = -1;
        Node* new_node = _NUM(tmp_d_neg_1);
        Node* right_node_copy = Copy_node((*root)->right_node);
        Tree_Dtor(*root);
        (*root) = _MUL(new_node, right_node_copy);

        return false;
    }

    return false;
}

bool Simplify_expression_right(Node** root, int64_t operation, double right_constant) {

    if(fabs(right_constant) < Epsilon && operation == MUL) {

        double tmp_d_zero = 0;
        Tree_Dtor(*root);
        *root = _NUM(tmp_d_zero);
        return true;
    }

    else if(fabs(right_constant - 1) < Epsilon && (operation == MUL ||
                                                   operation == DIV ||
                                                   operation == POW)
                                                                    ||
            fabs(right_constant)     < Epsilon && (operation == ADD ||
                                                   operation == SUB)) {

        Node* new_node = Copy_node((*root)->left_node);
        Tree_Dtor(*root);
        *root = new_node;
        return false;
    }

    else if(fabs(right_constant) < Epsilon && operation == POW) {

        double tmp_d_1 = 1;
        Tree_Dtor(*root);
        *root = _NUM(tmp_d_1);
        return true;
    }

    return false;
}
