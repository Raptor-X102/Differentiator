#include "Evaluation_funcs.h"

Node* Evaluate_const_node(Node* root) {

    Node_data* tmp_root_data = NULL, * tmp_node_data1 = NULL, * tmp_node_data2 = NULL;
    if(root)
        memcpy(&tmp_root_data, &root->node_data, sizeof(Node_data*));
    if(root->left_node)
        memcpy(&tmp_node_data1, &root->left_node->node_data, sizeof(Node_data*));
    if(root->right_node)
        memcpy(&tmp_node_data2, &root->right_node->node_data, sizeof(Node_data*));

    if(tmp_root_data->expression_type == OP) {

        if((tmp_root_data->value & is_binary) == is_binary) {

            if(tmp_node_data1->expression_type == NUM && tmp_node_data2->expression_type == NUM) {

                double tmp_double1 = 0, tmp_double2 = 0;
                memcpy(&tmp_double1, &tmp_node_data1->value, sizeof(double));
                memcpy(&tmp_double2, &tmp_node_data2->value, sizeof(double));
                double tmp_double_res = Do_arithmetics(tmp_root_data->value, tmp_double1, tmp_double2);
                DEBUG_PRINTF("\ntmp_double_res = %lg\n\n", tmp_double_res);
                return _NUM(tmp_double_res);
            }

            else
                return NULL;
        }

        else {

            if(tmp_node_data1->expression_type == NUM) {

                double tmp_double1 = 0, tmp_double2_zero = 0;
                memcpy(&tmp_double1, &tmp_node_data1->value, sizeof(double));
                double tmp_double_res = Do_arithmetics(tmp_root_data->value, tmp_double1, tmp_double2_zero);
                return _NUM(tmp_double_res);
            }

            else
                return NULL;
        }
    }

    else
        return NULL;
}

double Evaluate_tree(Var_list* var_list, Node* root) {

    Node_data* tmp_root_data = NULL, * tmp_node_data1 = NULL, * tmp_node_data2 = NULL;
    if(root)
        memcpy(&tmp_root_data, &root->node_data, sizeof(Node_data*));
    if(root->left_node)
        memcpy(&tmp_node_data1, &root->left_node->node_data, sizeof(Node_data*));
    if(root->right_node)
        memcpy(&tmp_node_data2, &root->right_node->node_data, sizeof(Node_data*));

    double tmp_double1 = 0, tmp_double2 = 0;

    if(tmp_node_data1->expression_type == OP)
        tmp_double1 = Evaluate_tree(var_list, root->left_node);

    if(tmp_node_data1->expression_type == VAR) {

        int64_t index = Find_variable(var_list, tmp_node_data1->value);
        if(index < 0) {

            DEBUG_PRINTF("ERROR: no such variable in var_list");
            return 0;
        }

        tmp_double1 = var_list->var_list[index].var_temp_value;
    }

    if(tmp_node_data1->expression_type == NUM)
        memcpy(&tmp_double1, &tmp_node_data1->value, sizeof(double));

    if((tmp_root_data->value & is_binary) == is_binary) {

        if(tmp_node_data2->expression_type == OP)
            tmp_double2 = Evaluate_tree(var_list, root->right_node);

        if(tmp_node_data2->expression_type == VAR) {

            int64_t index = Find_variable(var_list, tmp_node_data2->value);
            if(index < 0) {

                DEBUG_PRINTF("ERROR: no such variable in var_list");
                return 0;
            }

            tmp_double2 = var_list->var_list[index].var_temp_value;
        }

        if(tmp_node_data2->expression_type == NUM)
            memcpy(&tmp_double2, &tmp_node_data2->value, sizeof(double));
    }

    return Do_arithmetics(tmp_root_data->value, tmp_double1, tmp_double2);
}

double Do_arithmetics(int64_t operation, double num1, double num2) {

    switch (operation){

        case ADD:
            return num1 + num2;

        case SUB:
            return num1 - num2;

        case MUL:
            return num1 * num2;

        case DIV:

            if(num2 > Epsilon)
                return num1 / num2;

            else {

                DEBUG_PRINTF("ERROR: zero division\n");
                return 0;
            }

        case POW:

            if(num1 < Epsilon && num2 < Epsilon) {

                DEBUG_PRINTF("ERROR: 0 ^ 0\n");
                return 0;
            }

            else
                return pow(num1, num2);

        case LN:

            if(num1 > Epsilon)
                return log(num1);

            else {

                DEBUG_PRINTF("ERROR: ln(0)\n");
                return 0;
            }

        case SH:
            return sinh(num1);

        case CH:
            return cosh(num1);

        case TH:
            return tanh(num1);

        case LOG:

            if(num1 < Epsilon && num2 < Epsilon || (num1 - 1) < Epsilon) {

                DEBUG_PRINTF("ERROR: wrong log expression\n");
                return 0;
            }

            else
                return log(num1) / log(num2);

        case SIN:
            return sin(num1);

        case COS:
            return cos(num1);

        case TAN:

            if(fmod(num2, M_PI / 2) > Epsilon)
                return tan(num1);

            else {

                DEBUG_PRINTF("ERROR: tan((Pi/2) + (Pi * n))\n");
                return 0;
            }

        case COT:

            if(fmod(num2, M_PI) > Epsilon)
                return tan(M_PI / 2 - num1);

            else {

                DEBUG_PRINTF("ERROR: cot(Pi + (Pi * n))\n");
                return 0;
            }

        case EXP:
            return exp(num1);

        case CTH:
            return 1 / tanh(num1);

        case ASH:
            return asinh(num1);

        case ACH:
            return acosh(num1);

        case ATH:
            return atanh(num1);

        case ASIN:
            return asin(num1);

        case ACOS:
            return acos(num1);

        case ATAN:
            return atan(num1);

        case ACOT:
            return M_PI / 2 - atan(num1);

        case ACTH:
            return 0.5 * log((num1 + 1) / (num1 - 1));

        default:
            DEBUG_PRINTF("ERROR: no such function");
    }

    return 0;
}
