#include "Elementary_diff_funcs.h"

Node* Diff_ADD(Node* root) {

    Node* left_node = Diff_take_derivative(root->left_node);
    Node* right_node = Diff_take_derivative(root->right_node);
    return _ADD(left_node, right_node);
}

Node* Diff_SUB(Node* root) {

    Node* left_node = Diff_take_derivative(root->left_node);
    Node* right_node = Diff_take_derivative(root->right_node);
    return _SUB(left_node, right_node);
}

Node* Diff_MUL(Node* root) {

    Node* left_node_d = Diff_take_derivative(root->left_node);
    Node* left_node_c = Copy_node(root->left_node);
    Node* right_node_d = Diff_take_derivative(root->right_node);
    Node* right_node_c = Copy_node(root->right_node);

    Node* left_product = _MUL(left_node_d, right_node_c);
    Node* right_product = _MUL(right_node_d, left_node_c);
    return _ADD(left_product, right_product);
}

Node* Diff_DIV(Node* root) {

    Node* left_node_d = Diff_take_derivative(root->left_node);
    Node* left_node_c = Copy_node(root->left_node);
    Node* right_node_d = Diff_take_derivative(root->right_node);
    Node* right_node_c1 = Copy_node(root->right_node);
    Node* right_node_c2 = Copy_node(root->right_node);
    Node* right_node_c3 = Copy_node(root->right_node);

    Node* left_product = _MUL(left_node_d, right_node_c1);
    Node* right_product = _MUL(right_node_d, left_node_c);

    Node* numerator = _SUB(left_product, right_product);
    Node* denominator = _MUL(right_node_c2, right_node_c3);

    return _DIV(numerator, denominator);
}

Node* Diff_POW(Node* root) {

    if(Is_subtree_constant(root->left_node)) {

        DEBUG_PRINTF("Is_subtree_constant(root->left_node)\n");
        if(Is_subtree_constant(root->right_node)) {

            DEBUG_PRINTF("Is_subtree_constant(root->right_node)\n");
            double tmp_double = 0;
            return _NUM(tmp_double);
        }

        else { //exponent

            DEBUG_PRINTF("!Is_subtree_constant(root->right_node)\n");
            Node* base_copy = Copy_node(root->left_node);
            Node* ln_of_base = _LN(base_copy);
            Node* exponent_copy = Copy_node(root);
            Node* outer_derivative = _MUL(ln_of_base, exponent_copy);
            Node* inner_derivative = Diff_take_derivative(root->right_node);
            return _MUL(outer_derivative, inner_derivative);
        }
    }

    else {

        DEBUG_PRINTF("!Is_subtree_constant(root->left_node)\n");

        if(Is_subtree_constant(root->right_node)) { //y = x^k

            DEBUG_PRINTF("Is_subtree_constant(root->right_node)\n");

            double tmp_double = 1;
            Node* degree_decrement = _NUM(tmp_double);
            Node* degree_copy1 = Copy_node(root->right_node);
            Node* degree_copy2 = Copy_node(root->right_node);
            Node* degree = _SUB(degree_copy1, degree_decrement);
            Node* base_copy = Copy_node(root->left_node);
            Node* base_derivative = Diff_take_derivative(root->left_node);
            Node* new_degree = _POW(base_copy, degree);
            Node* outer_derivative = _MUL(degree_copy2, new_degree);
            return _MUL(outer_derivative, base_derivative);
        }

        else { // y = f(x)^g(x) = e^(g(x)*ln(f(x))); y' = y_cpy * (g(x)*ln(f(x)))' =

            DEBUG_PRINTF("!Is_subtree_constant(root->right_node)\n");

            Node* ln_base = _LN(root->left_node);
            Node* exp_degree = _MUL(root->right_node, ln_base);
            Node* exp_degree_derivative = Diff_take_derivative(exp_degree);
            Node* root_copy = Copy_node(root);

            Node_data* tmp_node_data1 = NULL;
            memcpy(&tmp_node_data1, &ln_base->node_data, sizeof(Node_data*));
            Node_data* tmp_node_data2 = NULL;
            memcpy(&tmp_node_data2, &exp_degree->node_data, sizeof(Node_data*));
            free(tmp_node_data1);
            free(tmp_node_data2);
            free(ln_base);
            free(exp_degree);

            return _MUL(root_copy, exp_degree_derivative);
        }
    }
}

Node* Diff_LN(Node* root) {

    double tmp_double = 1;
    Node* numerator = _NUM(tmp_double);
    Node* left_node_copy = Copy_node(root->left_node);
    Node* fraction = _DIV(numerator, left_node_copy);
    Node* inner_derivative = Diff_take_derivative(root->left_node);

    return _MUL(fraction, inner_derivative);
}

Node* Diff_LOG(Node* root) {

    if(Is_subtree_constant(root->left_node)) {

        double tmp_double = 1;
        Node* numerator = _NUM(tmp_double);
        Node* right_node_copy = Copy_node(root->right_node);
        Node* ln_base = _LN(right_node_copy);
        Node* left_node_copy = Copy_node(root->left_node);
        Node* denominator = _MUL(left_node_copy, ln_base);
        Node* fraction = _DIV(numerator, denominator);
        Node* inner_derivative = Diff_take_derivative(root->right_node);

        return _MUL(fraction, inner_derivative);
    }

    else {
        Node* ln_base = _LN(root->left_node);
        Node* ln_log_exp = _LN(root->right_node);
        Node* fraction = _DIV(ln_base, ln_log_exp);
        Node* log_derivative = Diff_take_derivative(fraction);

        Node_data* tmp_node_data1 = NULL;
        memcpy(&tmp_node_data1, &ln_base->node_data, sizeof(Node_data*));
        Node_data* tmp_node_data2 = NULL;
        memcpy(&tmp_node_data2, &ln_log_exp->node_data, sizeof(Node_data*));
        Node_data* tmp_node_data3 = NULL;
        memcpy(&tmp_node_data3, &fraction->node_data, sizeof(Node_data*));
        free(tmp_node_data1);
        free(tmp_node_data2);
        free(tmp_node_data3);
        free(ln_base);
        free(ln_log_exp);
        free(fraction);

        return log_derivative;
    }
}

Node* Diff_SIN(Node* root) {

    Node* left_node_copy = Copy_node(root->left_node);
    Node* outer_derivative = _COS(left_node_copy);
    Node* inner_derivative = Diff_take_derivative(root->left_node);

    return _MUL(outer_derivative, inner_derivative);
}

