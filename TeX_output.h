#ifndef TEX_OUTPUT
#define TEX_OUTPUT

#include "Diff_funcs.h"

#define TEX_PRINTF(...) fprintf(output_file, __VA_ARGS__)

char* Greek_alphabet[] = {"\\alpha", "\\beta",  "\\gamma",  "\\delta",   "\\epsilon", "\\zeta", "\\eta",     "\\theta",
                          "\\iota",  "\\kappa", "\\lambda", "\\mu",      "\\nu",      "\\xi",   "\\omikron", "\\pi",
                          "\\rho",   "\\sigma", "\\tau",    "\\upsilon", "\\phi",     "\\chi",  "\\psi",     "\\omega"};

void TeX_introduction(FILE* output_file);
void TeX_output_expr(const Node* root, FILE* output_file);
#endif
