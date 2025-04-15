#include "Elementary_diff_funcs.h"
#include "TeX_output.h"

Node* Diff_ADD(Node* root, FILE* output_file, uint64_t* replacements_count,
               bool write_explanation) {

    if(write_explanation) {

        TEX_PRINTF("\nDifferentiating sum: $(f(x) + g(x))' = f'(x) + g'(x)$\n");
        TEX_PRINTF("\\[");
        TeX_output_expr(root, output_file, true, replacements_count);
    }

    Node* left_node = Diff_take_derivative(root->left_node, output_file, replacements_count, write_explanation);
    Node* right_node = Diff_take_derivative(root->right_node, output_file, replacements_count, write_explanation);
    Node* diff_res = _ADD(left_node, right_node);

    //TEX_PRINTF("\\] \n");
    return diff_res;
}

Node* Diff_SUB(Node* root, FILE* output_file, uint64_t* replacements_count,
               bool write_explanation) {

    if(write_explanation) {

        TEX_PRINTF("\nDifferentiating substraction: $(f(x) - g(x))' = f'(x) - g'(x)$\n");
        TEX_PRINTF("\\[");
        TeX_output_expr(root, output_file, true, replacements_count);
    }

    Node* left_node = Diff_take_derivative(root->left_node, output_file, replacements_count, write_explanation);
    Node* right_node = Diff_take_derivative(root->right_node, output_file, replacements_count, write_explanation);
    return _SUB(left_node, right_node);
}

Node* Diff_MUL(Node* root, FILE* output_file, uint64_t* replacements_count,
               bool write_explanation) {

    if(write_explanation) {

        TEX_PRINTF("\nDifferentiating multiplication: $(f(x)g(x))' = f'(x)g(x) + f(x)g'(x)$\n");
        TEX_PRINTF("\\[");
        TeX_output_expr(root, output_file, true, replacements_count);
    }

    Node* left_node_d = Diff_take_derivative(root->left_node, output_file, replacements_count, write_explanation);
    Node* left_node_c = Copy_node(root->left_node);
    Node* right_node_d = Diff_take_derivative(root->right_node, output_file, replacements_count, write_explanation);
    Node* right_node_c = Copy_node(root->right_node);

    Node* left_product = _MUL(left_node_d, right_node_c);
    Node* right_product = _MUL(right_node_d, left_node_c);
    return _ADD(left_product, right_product);
}

Node* Diff_DIV(Node* root, FILE* output_file, uint64_t* replacements_count,
               bool write_explanation) {

    if(write_explanation) {

        TEX_PRINTF("\nDifferentiating division: $(\\frac{f(x)} {g(x)})' = \\frac{f'(x)g(x) - f(x)g'(x)}  {(g(x))^2}$\n");
        TEX_PRINTF("\\[");
        TeX_output_expr(root, output_file, true, replacements_count);
    }

    Node* left_node_d = Diff_take_derivative(root->left_node, output_file, replacements_count, write_explanation);
    Node* left_node_c = Copy_node(root->left_node);
    Node* right_node_d = Diff_take_derivative(root->right_node, output_file, replacements_count, write_explanation);
    Node* right_node_c1 = Copy_node(root->right_node);
    Node* right_node_c2 = Copy_node(root->right_node);
    Node* right_node_c3 = Copy_node(root->right_node);

    Node* left_product = _MUL(left_node_d, right_node_c1);
    Node* right_product = _MUL(right_node_d, left_node_c);

    Node* numerator = _SUB(left_product, right_product);
    Node* denominator = _MUL(right_node_c2, right_node_c3);

    return _DIV(numerator, denominator);
}

Node* Diff_POW(Node* root, FILE* output_file, uint64_t* replacements_count,
               bool write_explanation) {

    if(Is_subtree_constant(root->left_node)) {

        if(Is_subtree_constant(root->right_node)) {

            double tmp_double = 0;
            return _NUM(tmp_double);
        }

        else { //exponent

            if(write_explanation) {

                TEX_PRINTF("\nDifferentiating exponential: $(a^{f(x)})' = a^{f(x)} \\ln{a} f'(x)$\n");
                TEX_PRINTF("\\[");
                TeX_output_expr(root, output_file, true, replacements_count);
            }

            Node* base_copy = Copy_node(root->left_node);
            Node* ln_of_base = _LN(base_copy);
            Node* exponent_copy = Copy_node(root);
            Node* outer_derivative = _MUL(ln_of_base, exponent_copy);
            Node* inner_derivative = Diff_take_derivative(root->right_node, output_file, replacements_count, write_explanation);
            return _MUL(outer_derivative, inner_derivative);
        }
    }

    else {

        if(Is_subtree_constant(root->right_node)) { //y = x^k

            if(write_explanation) {

                TEX_PRINTF("\nDifferentiating degree: $({f(x)}^p)' = p {f(x)}^{p-1} f'(x)$\n");
                TEX_PRINTF("\\[");
                TeX_output_expr(root, output_file, true, replacements_count);
            }

            double tmp_double = 1;
            Node* degree_decrement = _NUM(tmp_double);
            Node* degree_copy1 = Copy_node(root->right_node);
            Node* degree_copy2 = Copy_node(root->right_node);
            Node* degree = _SUB(degree_copy1, degree_decrement);
            Node* base_copy = Copy_node(root->left_node);
            Node* base_derivative = Diff_take_derivative(root->left_node, output_file, replacements_count, write_explanation);
            Node* new_degree = _POW(base_copy, degree);
            Node* outer_derivative = _MUL(degree_copy2, new_degree);
            return _MUL(outer_derivative, base_derivative);
        }

        else { // y = f(x)^g(x) = e^(g(x)*ln(f(x))); y' = y_cpy * (g(x)*ln(f(x)))' =

            if(write_explanation) {

                TEX_PRINTF("\nDifferentiating degree-exponential: $({f(x)}^{g(x)})' = {f(x)}^{g(x)} ({g(x)} \\ln{f(x)})'$\n");
                TEX_PRINTF("\\[");
                TeX_output_expr(root, output_file, true, replacements_count);
            }
            Node* ln_base = _LN(root->left_node);
            Node* exp_degree = _MUL(root->right_node, ln_base);
            Node* exp_degree_derivative = Diff_take_derivative(exp_degree, output_file, replacements_count, write_explanation);
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

Node* Diff_LN(Node* root, FILE* output_file, uint64_t* replacements_count,
              bool write_explanation) {

    if(write_explanation) {

        TEX_PRINTF("\nDifferentiating ln: $(\\ln{f(x)})' = \\frac {f'(x)} {f(x)}$\n");
        TEX_PRINTF("\\[");
        TeX_output_expr(root, output_file, true, replacements_count);
    }

    double tmp_double = 1;
    Node* numerator = _NUM(tmp_double);
    Node* left_node_copy = Copy_node(root->left_node);
    Node* fraction = _DIV(numerator, left_node_copy);
    Node* inner_derivative = Diff_take_derivative(root->left_node, output_file, replacements_count, write_explanation);

    return _MUL(fraction, inner_derivative);
}

Node* Diff_LOG(Node* root, FILE* output_file, uint64_t* replacements_count,
               bool write_explanation) {

    if(Is_subtree_constant(root->left_node)) {

        if(write_explanation) {

            TEX_PRINTF("\nDifferentiating \\log_a {f(x)}: $(\\log_a {f(x)})' = \\frac {f'(x)} {f(x) ln{a}}$\n");
            TEX_PRINTF("\\[");
            TeX_output_expr(root, output_file, true, replacements_count);
        }

        double tmp_double = 1;
        Node* numerator = _NUM(tmp_double);
        Node* right_node_copy = Copy_node(root->right_node);
        Node* ln_base = _LN(right_node_copy);
        Node* left_node_copy = Copy_node(root->left_node);
        Node* denominator = _MUL(left_node_copy, ln_base);
        Node* fraction = _DIV(numerator, denominator);
        Node* inner_derivative = Diff_take_derivative(root->right_node, output_file, replacements_count, write_explanation);

        return _MUL(fraction, inner_derivative);
    }

    else {

        if(write_explanation) {

            TEX_PRINTF("\nDifferentiating \\log_{f(x)} {g(x)}: $(\\log_{f(x)} {g(x)})' = (\\frac {ln{g(x)} {ln{f(x)}})'$\n");
            TEX_PRINTF("\\[");
            TeX_output_expr(root, output_file, true, replacements_count);
        }

        Node* ln_base = _LN(root->left_node);
        Node* ln_log_exp = _LN(root->right_node);
        Node* fraction = _DIV(ln_base, ln_log_exp);
        Node* log_derivative = Diff_take_derivative(fraction, output_file, replacements_count, write_explanation);

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

Node* Diff_SIN(Node* root, FILE* output_file, uint64_t* replacements_count,
               bool write_explanation) {

    if(write_explanation) {

        TEX_PRINTF("\nDifferentiating sin: $(\\sin{f(x)})' = \\cos{f(x)} {f'(x)}$\n");
        TEX_PRINTF("\\[");
        TeX_output_expr(root, output_file, true, replacements_count);
    }

    Node* left_node_copy = Copy_node(root->left_node);
    Node* outer_derivative = _COS(left_node_copy);
    Node* inner_derivative = Diff_take_derivative(root->left_node, output_file, replacements_count, write_explanation);

    return _MUL(outer_derivative, inner_derivative);
}

Node* Diff_COS(Node* root, FILE* output_file, uint64_t* replacements_count,
               bool write_explanation) {

    if(write_explanation) {

        TEX_PRINTF("\nDifferentiating sin: $(\\cos{f(x)})' = -\\sin{f(x)} {f'(x)}$\n");
        TEX_PRINTF("\\[");
        TeX_output_expr(root, output_file, true, replacements_count);
    }

    Node* left_node_copy = Copy_node(root->left_node);
    double coeff = -1;
    Node* coeff_node = _NUM(coeff);
    Node* outer_derivative = _SIN(left_node_copy);
    Node* inner_derivative = Diff_take_derivative(root->left_node, output_file,
                                                  replacements_count, write_explanation);

    return _MUL(coeff_node, _MUL(outer_derivative, inner_derivative));
}

Node* Diff_TAN(Node* root, FILE* output_file, uint64_t* replacements_count,
               bool write_explanation) {

    if(write_explanation) {

        TEX_PRINTF("\nDifferentiating tan: $(\\tan{f(x)})' = \\frac{f'(x)} {{\\cos^{2}x}}$\n");
        TEX_PRINTF("\\[");
        TeX_output_expr(root, output_file, true, replacements_count);
    }

    Node* left_node_copy = Copy_node(root->left_node);
    double power = 2;
    Node* denominator = _POW(_COS(left_node_copy), _NUM(power));
    Node* inner_derivative = Diff_take_derivative(root->left_node, output_file,
                                                  replacements_count, write_explanation);

    return _DIV(inner_derivative, denominator);
}

Node* Diff_COT(Node* root, FILE* output_file, uint64_t* replacements_count,
               bool write_explanation) {

    if(write_explanation) {

        TEX_PRINTF("\nDifferentiating cot: $(\\cot{f(x)})' = \\frac{-f'(x)} {{\\sin^{2}x}}$\n");
        TEX_PRINTF("\\[");
        TeX_output_expr(root, output_file, true, replacements_count);
    }

    Node* left_node_copy = Copy_node(root->left_node);
    double numerator = -1;
    double power = 2;
    Node* numerator_node = _NUM(numerator);
    Node* denominator = _POW(_SIN(left_node_copy), _NUM(power));
    Node* inner_derivative = Diff_take_derivative(root->left_node, output_file,
                                                  replacements_count, write_explanation);
    inner_derivative = _MUL(numerator_node, inner_derivative);

    return _DIV(inner_derivative, denominator);
}

Node* Diff_SH(Node* root, FILE* output_file, uint64_t* replacements_count,
               bool write_explanation) {

    if(write_explanation) {

        TEX_PRINTF("\nDifferentiating sinh: $(\\sinh{f(x)})' = \\cosh{f(x)} {f'(x)}$\n");
        TEX_PRINTF("\\[");
        TeX_output_expr(root, output_file, true, replacements_count);
    }

    Node* left_node_copy = Copy_node(root->left_node);
    Node* outer_derivative = _CH(left_node_copy);
    Node* inner_derivative = Diff_take_derivative(root->left_node, output_file,
                                                  replacements_count, write_explanation);

    return _MUL(outer_derivative, inner_derivative);
}

Node* Diff_CH(Node* root, FILE* output_file, uint64_t* replacements_count,
               bool write_explanation) {

    if(write_explanation) {

        TEX_PRINTF("\nDifferentiating cosh: $(\\cosh{f(x)})' = \\sinh{f(x)} {f'(x)}$\n");
        TEX_PRINTF("\\[");
        TeX_output_expr(root, output_file, true, replacements_count);
    }

    Node* left_node_copy = Copy_node(root->left_node);
    Node* outer_derivative = _SH(left_node_copy);
    Node* inner_derivative = Diff_take_derivative(root->left_node, output_file,
                                                  replacements_count, write_explanation);

    return _MUL(outer_derivative, inner_derivative);
}

Node* Diff_TH(Node* root, FILE* output_file, uint64_t* replacements_count,
               bool write_explanation) {

    if(write_explanation) {

        TEX_PRINTF("\nDifferentiating tanh: $(\\tanh{f(x)})' = \\frac{f'(x)} {{\\cosh^{2}x}}$\n");
        TEX_PRINTF("\\[");
        TeX_output_expr(root, output_file, true, replacements_count);
    }

    Node* left_node_copy = Copy_node(root->left_node);
    double power = 2;
    Node* denominator = _POW(_CH(left_node_copy), _NUM(power));
    Node* inner_derivative = Diff_take_derivative(root->left_node, output_file,
                                                  replacements_count, write_explanation);

    return _DIV(inner_derivative, denominator);
}

Node* Diff_CTH(Node* root, FILE* output_file, uint64_t* replacements_count,
               bool write_explanation) {

    if(write_explanation) {

        TEX_PRINTF("\nDifferentiating coth: $(\\coth{f(x)})' = \\frac{-f'(x)} {{\\sinh^{2}x}}$\n");
        TEX_PRINTF("\\[");
        TeX_output_expr(root, output_file, true, replacements_count);
    }

    Node* left_node_copy = Copy_node(root->left_node);
    double numerator = -1;
    double power = 2;
    Node* numerator_node = _NUM(numerator);
    Node* denominator = _POW(_SH(left_node_copy), _NUM(power));
    Node* inner_derivative = Diff_take_derivative(root->left_node, output_file,
                                                  replacements_count, write_explanation);
    inner_derivative = _MUL(numerator_node, inner_derivative);
    return _DIV(inner_derivative, denominator);
}

Node* Diff_ASIN(Node* root, FILE* output_file, uint64_t* replacements_count,
               bool write_explanation) {

    if(write_explanation) {

        TEX_PRINTF("\nDifferentiating arcsin: $(\\arcsin{f(x)})' = \\frac{f'(x)} {\\sqrt{1-{f(x)}^2}}$\n");
        TEX_PRINTF("\\[");
        TeX_output_expr(root, output_file, true, replacements_count);
    }

    Node* left_node_copy = Copy_node(root->left_node);
    double tmp_double = 1;
    double power = 2;
    Node* denominator = _SQRT(_SUB(_NUM(tmp_double), _POW(left_node_copy, _NUM(power))));
    Node* inner_derivative = Diff_take_derivative(root->left_node, output_file,
                                                  replacements_count, write_explanation);

    return _DIV(inner_derivative, denominator);
}

Node* Diff_ACOS(Node* root, FILE* output_file, uint64_t* replacements_count,
               bool write_explanation) {

    if(write_explanation) {

        TEX_PRINTF("\nDifferentiating arccos: $(\\arccos{f(x)})' = \\frac{-f'(x)} {\\sqrt{1-{f(x)}^2}}$\n");
        TEX_PRINTF("\\[");
        TeX_output_expr(root, output_file, true, replacements_count);
    }

    Node* left_node_copy = Copy_node(root->left_node);
    double numerator = -1, tmp_double = 1;
    double power = 2;
    Node* numerator_node = _NUM(numerator);
    Node* denominator = _SQRT(_SUB(_NUM(tmp_double), _POW(left_node_copy, _NUM(power))));
    Node* inner_derivative = Diff_take_derivative(root->left_node, output_file,
                                                  replacements_count, write_explanation);
    inner_derivative = _MUL(numerator_node, inner_derivative);
    return _DIV(inner_derivative, denominator);
}

Node* Diff_ATAN(Node* root, FILE* output_file, uint64_t* replacements_count,
               bool write_explanation) {

    if(write_explanation) {

        TEX_PRINTF("\nDifferentiating arctan: $(\\arctan{f(x)})' = \\frac{f'(x)} {1 + {(f(x))}^2}$\n");
        TEX_PRINTF("\\[");
        TeX_output_expr(root, output_file, true, replacements_count);
    }

    Node* left_node_copy = Copy_node(root->left_node);
    double tmp_double = 1;
    double power = 2;
    Node* denominator = _ADD(_NUM(tmp_double), _POW(left_node_copy, _NUM(power)));
    Node* inner_derivative = Diff_take_derivative(root->left_node, output_file,
                                                  replacements_count, write_explanation);

    return _DIV(inner_derivative, denominator);
}

Node* Diff_ACOT(Node* root, FILE* output_file, uint64_t* replacements_count,
               bool write_explanation) {

    if(write_explanation) {

        TEX_PRINTF("\nDifferentiating arccot: $(\\arccot{f(x)})' = \\frac{-f'(x)} {1 + {f(x)}^2}$\n");
        TEX_PRINTF("\\[");
        TeX_output_expr(root, output_file, true, replacements_count);
    }

    Node* left_node_copy = Copy_node(root->left_node);
    double numerator = -1, tmp_double = 1;
    double power = 2;
    Node* denominator = _ADD(_NUM(tmp_double), _POW(left_node_copy, _NUM(power)));
    Node* inner_derivative = Diff_take_derivative(root->left_node, output_file,
                                                  replacements_count, write_explanation);
    inner_derivative = _MUL(_NUM(numerator), inner_derivative);
    return _DIV(inner_derivative, denominator);
}

Node* Diff_SQRT(Node* root, FILE* output_file, uint64_t* replacements_count,
               bool write_explanation) {

    if(write_explanation) {

        TEX_PRINTF("\nDifferentiating sqrt: $(\\sqrt{f(x)})' = \\frac{f'(x)} {2 {sqrt{f(x)}}$\n");
        TEX_PRINTF("\\[");
        TeX_output_expr(root, output_file, true, replacements_count);
    }

    Node* left_node_copy = Copy_node(root->left_node);
    double coeff = 2;
    Node* denominator = _MUL(_NUM(coeff), _SQRT(left_node_copy));
    Node* inner_derivative = Diff_take_derivative(root->left_node, output_file,
                                                  replacements_count, write_explanation);

    return _DIV(inner_derivative, denominator);
}

Node* Diff_ASH(Node* root, FILE* output_file, uint64_t* replacements_count,
               bool write_explanation) {

    if(write_explanation) {

        TEX_PRINTF("\nDifferentiating arcsin: $(\\arcsinh{f(x)})' = \\frac{f'(x)} {\\sqrt{{f(x)}^2} + 1}$\n");
        TEX_PRINTF("\\[");
        TeX_output_expr(root, output_file, true, replacements_count);
    }

    Node* left_node_copy = Copy_node(root->left_node);
    double tmp_double = 1;
    double power = 2;
    Node* denominator = _SQRT(_ADD(_POW(left_node_copy, _NUM(power)), _NUM(tmp_double)));
    Node* inner_derivative = Diff_take_derivative(root->left_node, output_file,
                                                  replacements_count, write_explanation);

    return _DIV(inner_derivative, denominator);
}

Node* Diff_ACH(Node* root, FILE* output_file, uint64_t* replacements_count,
               bool write_explanation) {

    if(write_explanation) {

        TEX_PRINTF("\nDifferentiating arcsin: $(\\arccosh{f(x)})' = \\frac{f'(x)} {\\sqrt{{f(x)}^2} - 1}$\n");
        TEX_PRINTF("\\[");
        TeX_output_expr(root, output_file, true, replacements_count);
    }

    Node* left_node_copy = Copy_node(root->left_node);
    double tmp_double = 1;
    double power = 2;
    Node* denominator = _SQRT(_SUB(_POW(left_node_copy, _NUM(power)), _NUM(tmp_double)));
    Node* inner_derivative = Diff_take_derivative(root->left_node, output_file,
                                                  replacements_count, write_explanation);

    return _DIV(inner_derivative, denominator);
}

Node* Diff_ATH(Node* root, FILE* output_file, uint64_t* replacements_count,
               bool write_explanation) {

    if(write_explanation) {

        TEX_PRINTF("\nDifferentiating arctanh: $(\\arctanh{f(x)})' = \\frac{f'(x)} {1 - {(f(x))}^2}$\n");
        TEX_PRINTF("\\[");
        TeX_output_expr(root, output_file, true, replacements_count);
    }

    Node* left_node_copy = Copy_node(root->left_node);
    double tmp_double = 1;
    double power = 2;
    Node* denominator = _SUB(_NUM(tmp_double), _POW(left_node_copy, _NUM(power)));
    Node* inner_derivative = Diff_take_derivative(root->left_node, output_file,
                                                  replacements_count, write_explanation);

    return _DIV(inner_derivative, denominator);
}

Node* Diff_ACTH(Node* root, FILE* output_file, uint64_t* replacements_count,
               bool write_explanation) {

    if(write_explanation) {

        TEX_PRINTF("\nDifferentiating arccoth: $(\\arccoth{f(x)})' = \\frac{f'(x)} {1 - {(f(x))}^2}$\n");
        TEX_PRINTF("\\[");
        TeX_output_expr(root, output_file, true, replacements_count);
    }

    Node* left_node_copy = Copy_node(root->left_node);
    double tmp_double = 1;
    double power = 2;
    Node* denominator = _SUB(_NUM(tmp_double), _POW(left_node_copy, _NUM(power)));
    Node* inner_derivative = Diff_take_derivative(root->left_node, output_file,
                                                  replacements_count, write_explanation);

    return _DIV(inner_derivative, denominator);
}

Node* Diff_ABS(Node* root, FILE* output_file, uint64_t* replacements_count,
               bool write_explanation) {

    if(write_explanation) {

        TEX_PRINTF("\nDifferentiating abs: $(|{f(x)}|)' = \\frac{f(x)} {|f(x)|} {f'(x)}$\n");
        TEX_PRINTF("\\[");
        TeX_output_expr(root, output_file, true, replacements_count);
    }

    Node* left_node_copy1 = Copy_node(root->left_node),
        * left_node_copy2 = Copy_node(root->left_node);
    Node* inner_derivative = Diff_take_derivative(root->left_node, output_file,
                                                  replacements_count, write_explanation);

    return _MUL(_DIV(left_node_copy1, _ABS(left_node_copy2)), inner_derivative);
}
