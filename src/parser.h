#ifndef PARSER_H
#define PARSER_H

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
// sadly, i cant make these pointers due to stack use after free :(
typedef struct node_return_t {
	node_int_lit_t int_lit_node;
} node_return_t;

typedef struct node_base_t {
	node_type type;
	union {
		node_int_lit_t int_lit_node;
		node_return_t return_node;
	};
} node_base_t;

node_base_t parse(LIST(token_t) tokens);
node_int_lit_t parse_int_lit(LIST(token_t) tokens, size_t *i);
node_return_t parse_return(LIST(token_t) tokens, size_t *i);

#endif // PARSER_H

#ifdef PARSER_IMPL

node_base_t parse(LIST(token_t) tokens) {
	node_base_t base_node;
	for(size_t i = 0; i < tokens.length; i++) {
		switch(tokens.value[i].type) {
		case token_keyword_return:
			base_node.type = node_return;
			base_node.return_node = parse_return(tokens, &i);
			break;
		}
	}
	
	return base_node;
}

node_int_lit_t parse_int_lit(LIST(token_t) tokens, size_t *i) {
	printf("parse_int_lit() start\n");
	if(*i >= tokens.length) {
		error(error_missing_expression, "your missing a number after the return");
	}
	if(tokens.value[*i].type != token_int_literal) {
		error(error_invalid_expression, "there is supposed to be a number after the return");
	}
	node_int_lit_t node = (node_int_lit_t){
		.token = tokens.value[*i]
	};
	printf("parse_int_lit() end\n");
	
	return node;
}

node_return_t parse_return(LIST(token_t) tokens, size_t *i) {
	printf("parse_return() start\n");
	++*i;
	node_return_t node = (node_return_t){
		.int_lit_node = parse_int_lit(tokens, i)
	};
	++*i;
	if(*i >= tokens.length) {
		error(error_missing_semicolon, "bro you missed a semicolon at the end of the return");
	}
	
	printf("parse_return() end\n");

	return node;
}

#endif // PARSER_IMPL
