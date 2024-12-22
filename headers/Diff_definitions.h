#ifndef DIFF_DEFS
#define DIFF_DEFS

#include "..\Binary_Trees\Tree_definitions.h"

struct Node_data {

    int expression_type;
    int64_t value;
};

enum Expression_type {

    NUM = 0x1,
    VAR = 0x2,
    OP = 0x3,
    SPECIAL_SYMBOL = 0xd15ab1ed
};

enum Operation_type_masks {

    is_binary = 0x1
};

const int64_t ADD  = 0x1,
              SUB  = 0x3,
              MUL  = 0x5,
              DIV  = 0x7,
              POW  = 0x9,

              LN   = 0xC,
              SH   = 0x18,
              CH   = 0x1A,
              TH   = 0x1C,

              LOG  = 0x2,
              SIN  = 0x4,
              COS  = 0x6,
              TAN  = 0x8,
              COT  = 0xA,
              EXP  = 0xE,
              CTH  = 0x1E,
              ASH  = 0x20,
              ACH  = 0x22,
              ATH  = 0x24,

              ASIN = 0x10,
              ACOS = 0x12,
              ATAN = 0x14,
              ACOT = 0x16,
              ACTH = 0x26,

              BRACKET_OP = 0xFA,
              BRACKET_CL = 0xFB,
              COMMA = 0xFC,
              EOT = 0xFF;

const int64_t X_var = 'x',
              Y_var = 'y',
              Z_var = 'z';

#endif
