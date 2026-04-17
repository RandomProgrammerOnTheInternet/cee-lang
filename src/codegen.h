#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>
#include <string.h>
#include "ds.h"
#include "lexer.h"
#include "parser.h"

FILE *generate_asm(LIST(node_base_t) node);
void generate_return(LIST(node_base_t) node, FILE **file, size_t *i);
void generate_var_decl(LIST(node_base_t) node, FILE **file, size_t *i);
void generate_label(LIST(node_base_t) node, FILE **file, size_t *i);
void generate_goto(LIST(node_base_t) node, FILE **file, size_t *i);
void generate_assignment(LIST(node_base_t) node, FILE **file, size_t *i);
void generate_bin_expr(node_expr_t expr, FILE **file, size_t *i, bool is_start);
#endif // CODEGEN_H
