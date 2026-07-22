#include "parser.h"

size_t stack_size = 0;
LIST(scope_t) scopes;
#define curr_scope scopes.value[scopes.length - 1]
FILE *tree;
size_t tree_offset = 0;

void print_offset() {
	for(size_t i = 0; i < tree_offset; i++) {
		fprintf(tree, "    ");
	}
}

LIST(node_base_t) parse(LIST(token_t) tokens) {
	LOG(PRN_GRN, "start");
	LIST(node_base_t) base_node;
	INIT_LIST(base_node, 0);

	INIT_LIST(scopes, 1); // scopes.value[0] is the global scope
	INIT_LIST(scopes.value[0].vars, 0);

	tree = fopen("out.ast", "w");
	fprintf(tree, "base\n");

	for(size_t i = 0; i < tokens.length; i++) {
		LOG(PRN_GRN, "loop");
		switch(tokens.value[i].type) {
		case token_keyword_goto: [[fallthrough]];
		case token_keyword_int: [[fallthrough]];
		case token_keyword_return: [[fallthrough]];
		case token_op_left_curly_brace: [[fallthrough]];
		case token_identifier:
			LOG(PRN_GRN, "detected statement %s", tokens.value[i].value);
			LIST_APPEND(base_node, ((node_base_t) {
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
	fclose(tree);
	LOG(PRN_GRN, "end");
	return base_node;
}

node_int_lit_t *parse_int_lit(LIST(token_t) tokens, size_t *i) {
	LOG(PRN_GRN, "start");
	tree_offset++;
	print_offset();
	fprintf(tree, "int_lit: %s\n", tokens.value[*i].value);
	node_int_lit_t *node = malloc(sizeof(node_int_lit_t));
	if(tokens.value[*i].type != token_int_literal) {
		LOG(PRN_GRN, "ERROR");
	}
	*node = (node_int_lit_t) {
		.token = tokens.value[*i]
	};
	
	tree_offset--;
	LOG(PRN_GRN, "end");
	return node;
}

node_return_t *parse_return(LIST(token_t) tokens, size_t *i) {
	LOG(PRN_GRN, "start");
	tree_offset++;
	print_offset();
	fprintf(tree, "return\n");
	++*i;
	node_return_t *node = malloc(sizeof(node_return_t));
	*node = (node_return_t) {
		.expr_node = parse_expr(tokens, i)
	};
	++*i;
	if(tokens.value[*i].type != token_op_semicolon) {
		LOG(PRN_GRN, "ERROR");
	}

	tree_offset--;
	LOG(PRN_GRN, "end");
	return node;
}

node_var_t parse_var(LIST(token_t) tokens, size_t *i) {
	LOG(PRN_GRN, "start");
	tree_offset++;
	print_offset();
	LOG(PRN_GRN, "curr_scope->vars.length = %zu", curr_scope.vars.length);
	if(curr_scope.vars.length == 0) {
		printf("error no variables in the scope");
		exit(1);
	}
	for(size_t j = 0; j < curr_scope.vars.length; j++) {
		LOG(PRN_GRN, "loop: %s", curr_scope.vars.value[j].token.value);
		if(!strcmp(tokens.value[*i].value, curr_scope.vars.value[j].token.value)) {
			fprintf(tree, "var: %s %zu\n", tokens.value[*i].value, curr_scope.vars.value[j].stack_offset);
			LOG(PRN_GRN, "end");
			return curr_scope.vars.value[j];
		}
	}
	tree_offset--;
	LOG(PRN_GRN, "ERROR");
	exit(1);
}

node_prim_expr_t *parse_prim_expr(LIST(token_t) tokens, size_t *i) {
	LOG(PRN_GRN, "start");
	tree_offset++;
	print_offset();
	fprintf(tree, "prim_expr\n");
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
	tree_offset--;
	LOG(PRN_GRN, "end");
	return node;
}

node_expr_t *parse_expr(LIST(token_t) tokens, size_t *i) {
	LOG(PRN_GRN, "start");
	tree_offset++;
	print_offset();
	fprintf(tree, "expr\n");
	node_expr_t *node = malloc(sizeof(node_expr_t));
	node->equal_expr_node = parse_equal_expr(tokens, i);

	tree_offset--;
	LOG(PRN_GRN, "end");
	return node;
}

node_mul_expr_t *parse_mul_expr(LIST(token_t) tokens, size_t *i) {
	LOG(PRN_GRN, "start");
	tree_offset++;
	print_offset();
	fprintf(tree, "mul_expr\n");
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
		tree_offset--;
		LOG(PRN_GRN, "end");
		return node;
	}
}

node_add_expr_t *parse_add_expr(LIST(token_t) tokens, size_t *i) {
	LOG(PRN_GRN, "start");
	tree_offset++;
	print_offset();
	fprintf(tree, "add_expr\n");
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

		tree_offset--;
		--*i;
		LOG(PRN_GRN, "end");
		return node;
	}
}

node_equal_expr_t *parse_equal_expr(LIST(token_t) tokens, size_t *i) {
	LOG(PRN_GRN, "start");
	tree_offset++;
	print_offset();
	fprintf(tree, "equal_expr\n");
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
		tree_offset--;
		--*i;
		LOG(PRN_GRN, "end");
		return node;
	}
}

node_var_decl_t *parse_var_decl(LIST(token_t) tokens, size_t *i) {
	LOG(PRN_GRN, "start");
	tree_offset++;
	print_offset();
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
	stack_size += sizeof(int);
	fprintf(tree, "var_decl: %s %zu\n", ident.value, stack_size);
	node_expr_t *expr = parse_expr(tokens, i);
	++*i;
	if(tokens.value[*i].type != token_op_semicolon) {
		LOG(PRN_GRN, "ERROR");
		exit(1);
	}
	node_var_t var_node = (node_var_t) {
		.stack_offset = stack_size,
		.token = ident
	};
	LIST_APPEND(curr_scope.vars, var_node);
	LOG(PRN_GRN, "var_node set");
	
	node_var_decl_t *decl_node = malloc(sizeof(node_var_decl_t));
	*decl_node = (node_var_decl_t) {
		.token = ident,
		.expr_node = expr,
		.stack_offset = stack_size
	};

	tree_offset--;
	LOG(PRN_GRN, "end");
	return decl_node;
}

node_label_t *parse_label(LIST(token_t) tokens, size_t *i) {
	LOG(PRN_GRN, "start");
	tree_offset++;
	print_offset();
	fprintf(tree, "label: %s\n", tokens.value[*i].value);
	++*i;
	node_label_t *node = malloc(sizeof(node_label_t));
	*node = (node_label_t) {
		.token = tokens.value[*i - 1]
	};

	tree_offset--;
	LOG(PRN_GRN, "end");
	return node;
}

node_goto_t *parse_goto(LIST(token_t) tokens, size_t *i) {
	LOG(PRN_GRN, "start");
	tree_offset++;
	print_offset();
	fprintf(tree, "goto: %s\n", tokens.value[*i + 1].value);
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

	tree_offset--;
	LOG(PRN_GRN, "end");
	return node;
}

node_assignment_t *parse_assignment(LIST(token_t) tokens, size_t *i) {
	LOG(PRN_GRN, "start");
	tree_offset++;
	print_offset();
	fprintf(tree, "assignment\n");
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

	tree_offset--;
	LOG(PRN_GRN, "end");
	return node;
}

node_compound_statement_t *parse_compound_statement(LIST(token_t) tokens, size_t *i) {
	LOG(PRN_GRN, "start");
	tree_offset++;
	print_offset();
	fprintf(tree, "compound_statement\n");
	node_compound_statement_t *node = malloc(sizeof(node_compound_statement_t));
	INIT_LIST(node->statement_nodes, 0);
	scope_t scope;
	INIT_LIST(scope.vars, curr_scope.vars.length);
	memcpy(scope.vars.value, curr_scope.vars.value, curr_scope.vars.length * sizeof(node_var_t));
	LIST_APPEND(scopes, scope);

	++*i;
	while(tokens.value[*i].type != token_op_right_curly_brace) {
		LOG(PRN_GRN, "loop %s", tokens.value[*i].value);
		LIST_APPEND(node->statement_nodes, parse_statement(tokens, i));
		++*i;
	}
	LOG(PRN_GRN, "loop end");
	LIST_ADD(scopes, -1); // remove scope

	tree_offset--;
	LOG(PRN_GRN, "end");
	return node;
}

node_statement_t *parse_statement(LIST(token_t) tokens, size_t *i) {
	LOG(PRN_GRN, "start");
	tree_offset++;
	print_offset();
	fprintf(tree, "statement\n");
	node_statement_t *node = malloc(sizeof(node_statement_t));
	switch(tokens.value[*i].type) {
	case token_keyword_return:
		LOG(PRN_GRN, "detected keyword return");
		*node = (node_statement_t) {
			.type = node_return,
			.return_node = parse_return(tokens, i)
		};
		tree_offset--;
		LOG(PRN_GRN, "end return");
		return node;
	case token_keyword_goto:
		LOG(PRN_GRN, "detected keyword goto");
		*node = (node_statement_t) {
			.type = node_goto,
			.goto_node = parse_goto(tokens, i)
		};
		tree_offset--;
		LOG(PRN_GRN, "end goto");
		return node;
	case token_keyword_int:
		LOG(PRN_GRN, "detected keyword int");
		*node = (node_statement_t) {
			.type = node_var_decl,
			.var_decl_node = parse_var_decl(tokens, i)
		};
		tree_offset--;
		LOG(PRN_GRN, "end int");
		return node;
	case token_op_left_curly_brace:
		LOG(PRN_GRN, "detected op_left_curly_brace");
		*node = (node_statement_t) {
			.type = node_compound_statement,
			.compound_statement_node = parse_compound_statement(tokens, i)
		};
		tree_offset--;
		LOG(PRN_GRN, "end left curly");
		return node;
	case token_identifier:
		LOG(PRN_GRN, "token_identifier: %s", tokens.value[*i].value);
		if(identifier_is_var(tokens.value[*i])) {
			LOG(PRN_GRN, "identifier is var");
			*node = (node_statement_t) {
				.type = node_assignment,
				.assignment_node = parse_assignment(tokens, i)
			};
			tree_offset--;
			return node;
		}
		else if(tokens.value[*i + 1].type == token_op_colon) {
			LOG(PRN_GRN, "identifier is label");
			*node = (node_statement_t) {
				.type = node_label,
				.label_node = parse_label(tokens, i)
			};
			tree_offset--;
			return node;
		}
		else {
			LOG(PRN_GRN, "ERROR");
			exit(1);
		}
	default:
		LOG(PRN_GRN, "ERROR %s", tokens.value[*i].value);
		exit(1);
	}
	printf("impossible error parse_statement\n");
	exit(1);
}

bool identifier_is_var(token_t token) {
	LOG(PRN_GRN, "start");
	for(size_t i = 0; i < curr_scope.vars.length; i++) {
		if(!strcmp(token.value, curr_scope.vars.value[i].token.value)) {
			LOG(PRN_GRN, "identifier %s is var", token.value);
			LOG(PRN_GRN, "end");
			return true;
		}
	}
	LOG(PRN_GRN, "identifier %s is not var", token.value);
	LOG(PRN_GRN, "end");
	return false;
}
