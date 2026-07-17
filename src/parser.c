#include "parser.h"

LIST(node_var_t) variable_lookup;
size_t stack_size = 0;

LIST(node_base_t) parse(LIST(token_t) tokens) {
	LOG(PRN_GRN, "start");
	INIT_LIST(variable_lookup, 0);
	LIST(node_base_t) base_node;
	INIT_LIST(base_node, 0);

	for(size_t i = 0; i < tokens.length; i++) {
		LOG(PRN_GRN, "loop");
		switch(tokens.value[i].type) {
		case token_keyword_goto: [[fallthrough]];
		case token_keyword_int: [[fallthrough]];
		case token_keyword_return: [[fallthrough]];
		case token_identifier:
			LOG(PRN_GRN, "detected statement");
			LIST_APPEND(base_node, ((node_base_t) {
				.type = node_statement,
				.statement_node = parse_statement(tokens, &i)
			}));
			LOG(PRN_GRN, "added statement");
			break;
		case token_op_semicolon: break;
		default:
			LOG(PRN_GRN, "default");
			break;
		}
	}
	LOG(PRN_GRN, "end");
	return base_node;
}

node_int_lit_t *parse_int_lit(LIST(token_t) tokens, size_t *i) {
	LOG(PRN_GRN, "start");
	node_int_lit_t *node = malloc(sizeof(node_int_lit_t));
	if(tokens.value[*i].type != token_int_literal) {
		LOG(PRN_GRN, "ERROR");
	}
	*node = (node_int_lit_t) {
		.token = tokens.value[*i]
	};
	
	LOG(PRN_GRN, "end");
	return node;
}

node_return_t *parse_return(LIST(token_t) tokens, size_t *i) {
	LOG(PRN_GRN, "start");
	++*i;
	node_return_t *node = malloc(sizeof(node_return_t));
	*node = (node_return_t) {
		.expr_node = parse_expr(tokens, i)
	};
	++*i;
	if(tokens.value[*i].type != token_op_semicolon) {
		LOG(PRN_GRN, "ERROR");
	}

	LOG(PRN_GRN, "end");
	return node;
}

node_var_t parse_var(LIST(token_t) tokens, size_t *i) {
	LOG(PRN_GRN, "start");
	LOG(PRN_GRN, "variable_lookup.length = %zu", variable_lookup.length);
	for(size_t j = 0; j < variable_lookup.length; j++) {
		LOG(PRN_GRN, "loop: %s", variable_lookup.value[j].token.value);
		if(!strcmp(tokens.value[*i].value, variable_lookup.value[j].token.value)) {
			LOG(PRN_GRN, "end");
			return variable_lookup.value[j];
		}
	}
	LOG(PRN_GRN, "ERROR");
	exit(1);
}

node_prim_expr_t *parse_prim_expr(LIST(token_t) tokens, size_t *i) {
	LOG(PRN_GRN, "start");
	node_prim_expr_t *node = malloc(sizeof(node_prim_expr_t));
	switch(tokens.value[*i].type) {
	case token_int_literal:
		LOG(PRN_GRN, "int lit");
		*node = (node_prim_expr_t) {
			.type = node_int_lit,
			.int_lit_node = parse_int_lit(tokens, i)
		};
		break;
	case token_identifier:
		if(identifier_is_var(tokens.value[*i])) {
			*node = (node_prim_expr_t) {
				.type = node_var,
				.var_node = parse_var(tokens, i)
			};
		}
		break;
	default:
		LOG(PRN_GRN, "ERROR");
		break;
	}
	LOG(PRN_GRN, "end");
	return node;
}

node_expr_t *parse_expr(LIST(token_t) tokens, size_t *i) {
	LOG(PRN_GRN, "start");
	node_expr_t *node = malloc(sizeof(node_expr_t));
	node->equal_expr_node = parse_equal_expr(tokens, i);

	LOG(PRN_GRN, "end");
	return node;
}

node_mul_expr_t *parse_mul_expr(LIST(token_t) tokens, size_t *i) {
	LOG(PRN_GRN, "start");
	node_mul_expr_t *node = malloc(sizeof(node_mul_expr_t));
	*node = (node_mul_expr_t) {
		.type = node_prim_expr,
		.prim_expr_node = parse_prim_expr(tokens, i) 
	};

	while(1) {
		++*i;
		if(tokens.value[*i].type == token_op_asterisk) {
			++*i;
			node_mul_expr_t *tmp = malloc(sizeof(node_mul_expr_t));
			memcpy(tmp, node, sizeof(node_mul_expr_t));
			*node = (node_mul_expr_t) {
				.type = node_mul_expr,
				.op = op_mul,
				.lhs = tmp,
				.rhs = parse_prim_expr(tokens, i)
			};
			continue;
		}
		else if(tokens.value[*i].type == token_op_slash) {
			++*i;
			node_mul_expr_t *tmp = malloc(sizeof(node_mul_expr_t));
			memcpy(tmp, node, sizeof(node_mul_expr_t));
			*node = (node_mul_expr_t) {
				.type = node_mul_expr,
				.op = op_div,
				.lhs = tmp,
				.rhs = parse_prim_expr(tokens, i)
			};
			continue;
		}
		else if(tokens.value[*i].type == token_op_percent) {
			++*i;
			node_mul_expr_t *tmp = malloc(sizeof(node_mul_expr_t));
			memcpy(tmp, node, sizeof(node_mul_expr_t));
			*node = (node_mul_expr_t) {
				.type = node_mul_expr,
				.op = op_mod,
				.lhs = tmp,
				.rhs = parse_prim_expr(tokens, i)
			};
			continue;
		}
		--*i;
		LOG(PRN_GRN, "end");
		return node;
	}
}

node_add_expr_t *parse_add_expr(LIST(token_t) tokens, size_t *i) {
	LOG(PRN_GRN, "start");
	node_add_expr_t *node = malloc(sizeof(node_add_expr_t));
	*node = (node_add_expr_t) {
		.type = node_mul_expr,
		.mul_expr_node = parse_mul_expr(tokens, i)
	};
	while(1) {
		++*i;
		if(tokens.value[*i].type == token_op_plus) {
			++*i;
			node_add_expr_t *tmp = malloc(sizeof(node_add_expr_t));
			memcpy(tmp, node, sizeof(node_add_expr_t));
			*node = (node_add_expr_t) {
				.type = node_add_expr,
				.op = op_add,
				.lhs = tmp,
				.rhs = parse_mul_expr(tokens, i)
			};
			continue;
		}
		else if(tokens.value[*i].type == token_op_minus) {
			++*i;
			node_add_expr_t *tmp = malloc(sizeof(node_add_expr_t));
			memcpy(tmp, node, sizeof(node_add_expr_t));
			*node = (node_add_expr_t) {
				.type = node_add_expr,
				.op = op_sub,
				.lhs = tmp,
				.rhs = parse_mul_expr(tokens, i)
			};
			continue;
		}

		--*i;
		LOG(PRN_GRN, "end");
		return node;
	}
}

node_equal_expr_t *parse_equal_expr(LIST(token_t) tokens, size_t *i) {
	LOG(PRN_GRN, "start");
	node_equal_expr_t *node = malloc(sizeof(node_equal_expr_t));
	*node = (node_equal_expr_t) {
		.type = node_add_expr,
		.add_expr_node = parse_add_expr(tokens, i)
	};
	while(1) {
		++*i;
		if(tokens.value[*i].type == token_op_equals_equals) {
			++*i;
			node_equal_expr_t *tmp = malloc(sizeof(node_equal_expr_t));
			memcpy(tmp, node, sizeof(node_add_expr_t));
			*node = (node_equal_expr_t) {
				.type = node_equal_expr,
				.op = op_equ,
				.lhs = tmp,
				.rhs = parse_add_expr(tokens, i)
			};
			continue;
		}
		else if(tokens.value[*i].type == token_op_not_equals) {
			++*i;
			node_equal_expr_t *tmp = malloc(sizeof(node_equal_expr_t));
			memcpy(tmp, node, sizeof(node_add_expr_t));
			*node = (node_equal_expr_t) {
				.type = node_equal_expr,
				.op = op_neq,
				.lhs = tmp,
				.rhs = parse_add_expr(tokens, i)
			};
			continue;
		}
		--*i;
		LOG(PRN_GRN, "end");
		return node;
	}
}

node_var_decl_t *parse_var_decl(LIST(token_t) tokens, size_t *i) {
	LOG(PRN_GRN, "start");
	if(tokens.value[*i].type != token_keyword_int) {
		LOG(PRN_GRN, "ERROR");
		exit(1);
	}
	++*i;
	if(tokens.value[*i].type != token_identifier) {
		LOG(PRN_GRN, "ERROR");
		exit(1);
}
	token_t ident = tokens.value[*i];
	LOG(PRN_GRN, "identifier: %s", tokens.value[*i].value);
	++*i;
	if(tokens.value[*i].type != token_op_equals) {
		LOG(PRN_GRN, "ERROR");
		exit(1);
	}
	++*i;
	if(tokens.value[*i].type != token_int_literal &&
		tokens.value[*i].type != token_identifier) {
		LOG(PRN_GRN, "ERROR");
		exit(1);
	}
	node_expr_t *expr = parse_expr(tokens, i);
	++*i;
	if(tokens.value[*i].type != token_op_semicolon) {
		LOG(PRN_GRN, "ERROR");
		exit(1);
	}
	stack_size += sizeof(int);
	node_var_t var_node = (node_var_t) {
		.stack_offset = stack_size,
		.token = ident
	};
	LIST_APPEND(variable_lookup, var_node);
	LOG(PRN_GRN, "var_node set");
	
	node_var_decl_t *decl_node = malloc(sizeof(node_var_decl_t));
	*decl_node = (node_var_decl_t) {
		.token = ident,
		.expr_node = expr,
		.stack_offset = stack_size
	};

	LOG(PRN_GRN, "end");
	return decl_node;
}

node_label_t *parse_label(LIST(token_t) tokens, size_t *i) {
	LOG(PRN_GRN, "start");
	++*i;
	node_label_t *node = malloc(sizeof(node_label_t));
	*node = (node_label_t) {
		.token = tokens.value[*i - 1]
	};

	LOG(PRN_GRN, "end");
	return node;
}

node_goto_t *parse_goto(LIST(token_t) tokens, size_t *i) {
	LOG(PRN_GRN, "start");
	++*i;
	if(tokens.value[*i].type != token_identifier) {
		LOG(PRN_GRN, "ERROR");
		exit(1);
	}
	++*i;
	if(tokens.value[*i].type != token_op_semicolon) {
		LOG(PRN_GRN, "ERROR");
		exit(1);
	}
	node_goto_t *node = malloc(sizeof(node_goto_t));
	*node = (node_goto_t) {
		.token = tokens.value[*i - 1]
	};

	LOG(PRN_GRN, "end");
	return node;
}

node_assignment_t *parse_assignment(LIST(token_t) tokens, size_t *i) {
	LOG(PRN_GRN, "start");
	node_assignment_t *node = malloc(sizeof(node_assignment_t));
	node->lhs = parse_var(tokens, i);
	LOG(PRN_GRN, "set lhs");
	++*i;
	if(tokens.value[*i].type != token_op_equals) {
		LOG(PRN_GRN, "ERROR");
		exit(1);
	}
	++*i;
	node->rhs = parse_expr(tokens, i);
	LOG(PRN_GRN, "rhs set");

	LOG(PRN_GRN, "end");
	return node;
}

node_statement_t *parse_statement(LIST(token_t) tokens, size_t *i) {
	LOG(PRN_GRN, "start");
	node_statement_t *node = malloc(sizeof(node_statement_t));
	switch(tokens.value[*i].type) {
	case token_keyword_return:
		LOG(PRN_GRN, "detected keyword return");
		*node = (node_statement_t) {
			.type = node_return,
			.return_node = parse_return(tokens, i)
		};
		LOG(PRN_GRN, "end return");
		return node;
		break;
	case token_keyword_goto:
		LOG(PRN_GRN, "detected keyword goto");
		*node = (node_statement_t) {
			.type = node_goto,
			.goto_node = parse_goto(tokens, i)
		};
		LOG(PRN_GRN, "end goto");
		return node;
		break;
	case token_keyword_int:
		LOG(PRN_GRN, "detected keyword int");
		*node = (node_statement_t) {
			.type = node_var_decl,
			.var_decl_node = parse_var_decl(tokens, i)
		};
		LOG(PRN_GRN, "end int");
		return node;
		break;
	case token_identifier:
		LOG(PRN_GRN, "token_identifier: %s", tokens.value[*i].value);
		if(identifier_is_var(tokens.value[*i])) {
			LOG(PRN_GRN, "identifier is var");
			*node = (node_statement_t) {
				.type = node_assignment,
				.assignment_node = parse_assignment(tokens, i)
			};
			return node;
		}
		else if(tokens.value[*i + 1].type == token_op_colon) {
			LOG(PRN_GRN, "identifier is label");
			*node = (node_statement_t) {
				.type = node_label,
				.label_node = parse_label(tokens, i)
			};
			return node;
		}
		else {
			LOG(PRN_GRN, "ERROR");
			exit(1);
		}
		break;
	default:
		LOG(PRN_GRN, "ERROR");
		exit(1);
	}
	printf("impossible error parse_statement\n");
	exit(1);
}

bool identifier_is_var(token_t token) {
	LOG(PRN_GRN, "start");
	for(size_t i = 0; i < variable_lookup.length; i++) {
		if(!strcmp(token.value, variable_lookup.value[i].token.value)) {
			LOG(PRN_GRN, "identifier is var");
			LOG(PRN_GRN, "end");
			return true;
		}
	}
	LOG(PRN_GRN, "identifier is not var");
	LOG(PRN_GRN, "end");
	return false;
}
