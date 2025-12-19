#ifndef PARSER_H
#define PARSER_H

// git test

#include "ds.h"
#include "util.h"

typedef enum node_type {
	node_base = 0,
	node_return,
	node_int_lit,
	node_var,
	node_var_declaration,
	node_expression,
	node_statement
} node_type;

typedef enum var_type {
	var_char = 1,
	var_short = 2,
	var_int = 4,
	var_long = 8,
	var_long_long = 8,
	var_float = 4,
	var_double = 8,
	var_long_double = 8 // who uses these?
} var_type;

typedef struct node_int_lit_t {
	token_t token;
} node_int_lit_t;

typedef struct node_var_t {
	var_type type;
	size_t stack_offset;
	token_t token;
} node_var_t;

typedef struct node_expression_t {
	node_type type;
	union {
		node_int_lit_t int_lit_node;
		node_var_t var_node;
	};
} node_expression_t;

typedef struct node_var_declaration_t {
	bool has_value;
	var_type type;
	token_t token;
	size_t stack_offset;
	node_expression_t expression_node;
} node_var_declaration_t;

typedef struct node_return_t {
	node_expression_t expression_node;
} node_return_t;

typedef struct node_statement_t {
	node_type type;
	union {
		node_var_declaration_t var_declaration_node;
		node_return_t return_node;
	};
} node_statement_t;

typedef struct node_base_t {
	node_type type;
	union {
		node_statement_t statement_node;
		node_expression_t expression_node;
	};
} node_base_t;

NEW_LIST(node_var_t);
NEW_LIST(node_base_t);

size_t stack_size = 0;
static LIST(node_var_t) variable_lookup;

LIST(node_base_t) parse(LIST(token_t) tokens);
node_int_lit_t parse_int_lit(LIST(token_t) tokens, size_t *i);
node_return_t parse_return(LIST(token_t) tokens, size_t *i);
node_var_t parse_var(LIST(token_t) tokens, size_t *i);
node_expression_t parse_expression(LIST(token_t) tokens, size_t *i);
node_var_declaration_t parse_var_declaration(LIST(token_t) tokens, size_t *i); // oooh so close
node_statement_t parse_statement(LIST(token_t) tokens, size_t *i);

#endif // PARSER_H

#ifdef PARSER_IMPL

LIST(node_base_t) parse(LIST(token_t) tokens) {
	INIT_LIST(variable_lookup, 0);
	// trust me on this
	LIST(node_base_t) base_node;
	INIT_LIST(base_node, 0);
	for(size_t i = 0; i < tokens.length; i++) {
		switch(tokens.value[i].type) {
		case token_keyword_int: [[fallthrough]];
		case token_keyword_return:
			LIST_APPEND(base_node, ((node_base_t) {
				.type = node_statement,
				.statement_node = parse_statement(tokens, &i)
			}));
			break;
		}
	}
	
	return base_node;
}

node_int_lit_t parse_int_lit(LIST(token_t) tokens, size_t *i) {
	if(*i >= tokens.length) {
		error(error_missing_expression, "your missing a number after the return");
	}
	if(tokens.value[*i].type != token_int_literal) {
		error(error_invalid_expression, "there is supposed to be a number after the return");
	}
	node_int_lit_t node = (node_int_lit_t){
		.token = tokens.value[*i]
	};
	
	return node;
}

node_return_t parse_return(LIST(token_t) tokens, size_t *i) {
	++*i;
	node_return_t node = (node_return_t){
		.expression_node = parse_expression(tokens, i)
	};
	++*i;
	if(*i >= tokens.length) {
		error(error_missing_semicolon, "bro you missed a semicolon at the end of the return");
	}
	
	return node;
}

node_var_t parse_var(LIST(token_t) tokens, size_t *i) {
	size_t j;
	printf("%zu\n", variable_lookup.length);
	for(j = 0; j < variable_lookup.length; j++) {
		printf("%zu\n", j);
		if(strcmp(tokens.value[*i].value,
				  variable_lookup.value[j].token.value) == 0) {
			return variable_lookup.value[j];
		}
	}
	printf("uh oh!!\n");
	exit(1);
}

node_expression_t parse_expression(LIST(token_t) tokens, size_t *i) {
	switch(tokens.value[*i].type) {
	case token_int_literal:
		return (node_expression_t) {
			.type = node_int_lit,
			.int_lit_node = parse_int_lit(tokens, i)
		};
		break;
	case token_identifier:
		return (node_expression_t) {
			.type = node_var,
			.var_node = parse_var(tokens, i)
		};
		break;
	}
	printf("uh oh!\n");
	exit(1);
}

node_var_declaration_t parse_var_declaration(LIST(token_t) tokens, size_t *i) {
	++*i;
	if(tokens.value[*i].type != token_identifier) {
		printf("womp womp\n");
		exit(1);
	}
	++*i;
	if(tokens.value[*i].type == token_op_semicolon) {
		stack_size += var_int;
		LIST_APPEND(variable_lookup, ((node_var_t) {
			.type = var_int,
			.stack_offset = stack_size,
			.token = tokens.value[*i - 1],
		}));
		return (node_var_declaration_t) {
			.has_value = 0,
			.type = var_int,
			.token = tokens.value[*i - 1],
			.stack_offset = stack_size
		};
	}
	else if(tokens.value[*i].type == token_op_equals) {
		++*i;
		if(tokens.value[*i].type != token_int_literal) {
			printf("womp womp!\n");
			exit(1);
		}
		node_expression_t expression = parse_expression(tokens, i);
		++*i;
		if(tokens.value[*i].type != token_op_semicolon) {
			printf("skill issue\n");
			exit(1);
		}
		stack_size += var_int;
		LIST_APPEND(variable_lookup, ((node_var_t) {
			.type = var_int,
			.stack_offset = stack_size,
			.token = tokens.value[*i - 3],
		}));
		return (node_var_declaration_t) {
			.has_value = 1,
			.type = var_int,
			.token = tokens.value[*i - 3],
			.expression_node = expression,
			.stack_offset = stack_size
		};
	}
}

node_statement_t parse_statement(LIST(token_t) tokens, size_t *i) {
	switch(tokens.value[*i].type) {
	case token_keyword_return:
		return (node_statement_t) {
			.type = node_return,
			.return_node = parse_return(tokens, i)
		};
		break;
	case token_keyword_int:
		return (node_statement_t) {
			.type = node_var_declaration,
			.var_declaration_node = parse_var_declaration(tokens, i)
		};
		break;
	}
	printf("uh oh\n");
	exit(1);
}

#endif // PARSER_IMPL
