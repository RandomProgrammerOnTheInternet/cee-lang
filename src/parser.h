#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "ds.h"
#include "util.h"

typedef enum node_type {
	node_base = 0,
	node_return,
	node_int_lit
} node_type;

typedef struct node_int_lit_t {
	token_t token;
} node_int_lit_t;

typedef struct node_return_t {
	node_int_lit_t *int_lit;
} node_return_t;

typedef struct node_base_t {
	node_type type;
	union {
		node_int_lit_t *int_lit;
		node_return_t *return_t;
	};
} node_base_t;

node_base_t parse(LIST(token_t) tokens);

#endif // PARSER_H

#ifdef PARSER_IMPL

node_base_t parse(LIST(token_t) tokens) {
	node_base_t base_node;
	
	return base_node;
}

#endif // PARSER_IMPL
