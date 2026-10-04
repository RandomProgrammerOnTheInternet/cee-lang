#include "parser.h"

#define peek(tok) tokens.value[*i + 1].type == tok

#define expect(tok) \
do { \
	if(tokens.value[*i].type != tok) { \
		LOG(PRN_GRN, "ERROR: %s", tokens.value[*i + 1].value); \
		exit(1); \
	} \
	++*i; \
} while(0);

#define consume(tok) tokens.value[*i].type == tok ? ++*i : 0

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
	INIT_LIST(scopes.value[0].objs, 0);

	tree = fopen("out.ast", "w");
	fprintf(tree, "base\n");

	for(size_t i = 0; i < tokens.length; i++) {
		LOG(PRN_GRN, "loop");
		switch(tokens.value[i].type) {
		case token_keyword_fn:
			LOG(PRN_GRN, "detected fn %s", tokens.value[i + 1].value);
			LIST_APPEND(base_node, ((node_base_t) {
				.fn_def_node = parse_fn_def(tokens, &i)
			}));
			LOG(PRN_GRN, "added fn");
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

node_fn_def_t *parse_fn_def(LIST(token_t) tokens, size_t *i) {
	LOG(PRN_GRN, "start");
	tree_offset++;
	print_offset();
	stack_size = 0;
	++*i;
	expect(token_keyword_int);
	fprintf(tree, "fn: %s\n", tokens.value[*i].value);
	if(identifier_is_fn(tokens.value[*i]) || identifier_is_var(tokens.value[*i])) {
		LOG(PRN_GRN, "ERROR");
		exit(1);
	}
	obj_t fn;
	fn.is_fn = true;
	fn.token = tokens.value[*i];
	++*i;
	expect(token_op_left_paren);
	expect(token_op_right_paren);
	expect(token_op_left_curly_brace);
	--*i;
	LIST_APPEND(curr_scope.objs, fn);
	node_fn_def_t *fn_def = malloc(sizeof(node_fn_def_t));
	*fn_def = (node_fn_def_t) {
		.has_parameter_list = false,
		.token = fn.token,
		.compound_statement_node = parse_compound_statement(tokens, i)
	};

	tree_offset--;
	LOG(PRN_GRN, "end");
	return fn_def;
}


obj_t parse_var(LIST(token_t) tokens, size_t *i) {
	LOG(PRN_GRN, "start");
	tree_offset++;
	print_offset();
	LOG(PRN_GRN, "curr_scope->objs.length = %zu", curr_scope.objs.length);
	if(curr_scope.objs.length == 0) {
		printf("error no objects in the scope");
		exit(1);
	}
	for(size_t j = 0; j < curr_scope.objs.length; j++) {
		LOG(PRN_GRN, "loop: %s", curr_scope.objs.value[j].token.value);
		if(!strcmp(tokens.value[*i].value, curr_scope.objs.value[j].token.value) &&
		   curr_scope.objs.value[j].is_fn == false) {
			fprintf(tree, "obj: %s %zu\n", tokens.value[*i].value, curr_scope.objs.value[j].stack_offset);
			tree_offset--;
			LOG(PRN_GRN, "end");
			return curr_scope.objs.value[j];
		}
	}
	LOG(PRN_GRN, "ERROR");
	exit(1);
}

obj_t parse_fn(LIST(token_t) tokens, size_t *i) {
	LOG(PRN_GRN, "start");
	tree_offset++;
	print_offset();
	LOG(PRN_GRN, "curr_scope->objs.length = %zu", curr_scope.objs.length);
	if(curr_scope.objs.length == 0) {
		printf("error no objects in the scope");
		exit(1);
	}
	for(size_t j = 0; j < curr_scope.objs.length; j++) {
		LOG(PRN_GRN, "loop: %s", curr_scope.objs.value[j].token.value);
		if(!strcmp(tokens.value[*i].value, curr_scope.objs.value[j].token.value) &&
		   curr_scope.objs.value[j].is_fn == true) {
			fprintf(tree, "obj: %s %zu\n", tokens.value[*i].value, curr_scope.objs.value[j].stack_offset);
			tree_offset--;
			LOG(PRN_GRN, "end");
			return curr_scope.objs.value[j];
		}
	}
	LOG(PRN_GRN, "ERROR");
	exit(1);
}

node_expr_t *parse_expr(LIST(token_t) tokens, size_t *i) {
	LOG(PRN_GRN, "start");
	tree_offset++;
	print_offset();
	fprintf(tree, "expr\n");
	node_expr_t *node = malloc(sizeof(node_expr_t));
	node->assign_expr_node = parse_assign_expr(tokens, i);

	tree_offset--;
	LOG(PRN_GRN, "end");
	return node;
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
				.obj = parse_var(tokens, i)
			};
		}
		else if(identifier_is_fn(tokens.value[*i])) {
			*node = (node_prim_expr_t) {
				.type = node_fn_call,
				.obj = parse_fn(tokens, i)
			};
		}
		else {
			LOG(PRN_GRN, "ERROR: %s", tokens.value[*i].value);
			exit(1);
		}
		break;
	default:
		for(size_t j = *i - 5; j <= *i + 5; j++) {
			LOG(PRN_GRN, "%s", tokens.value[j].value);
		}
		LOG(PRN_GRN, "ERROR: %s", tokens.value[*i].value);
		exit(1);
	}
	LOG(PRN_GRN, "%s", tokens.value[*i].value);
	tree_offset--;
	LOG(PRN_GRN, "end");
	return node;
}

node_post_expr_t *parse_post_expr(LIST(token_t) tokens, size_t *i) {
	LOG(PRN_GRN, "start");
	tree_offset++;
	print_offset();
	fprintf(tree, "post_expr\n");
	node_post_expr_t *node = malloc(sizeof(node_post_expr_t));
	*node = (node_post_expr_t) {
		.type = node_prim_expr,
		.prim_expr_node = parse_prim_expr(tokens, i)
	};
	while(1) {
		++*i;
		LOG(PRN_GRN, "loop: %s", tokens.value[*i - 1].value);
		LOG(PRN_GRN, "loop: %s", tokens.value[*i].value);
		LOG(PRN_GRN, "loop: %s", tokens.value[*i + 1].value);
		if(tokens.value[*i].type == token_op_left_paren) {
			LOG(PRN_GRN, "tokens.value[*i].type == token_op_left_paren");
			++*i;
			node_post_expr_t *tmp = malloc(sizeof(node_post_expr_t));
			memcpy(tmp, node, sizeof(node_post_expr_t));
			*node = (node_post_expr_t) {
				.type = node_post_expr,
				.op = op_fun,
				.post_expr_node = tmp
			};
			continue;
		}
		else if(tokens.value[*i].type == token_op_plus_plus) {
			LOG(PRN_GRN, "tokens.value[*i].type == token_op_plus_plus");
			++*i;
			node_post_expr_t *tmp = malloc(sizeof(node_post_expr_t));
			memcpy(tmp, node, sizeof(node_post_expr_t));
			*node = (node_post_expr_t) {
				.type = node_post_expr,
				.op = op_inc,
				.post_expr_node = tmp
			};
			continue;
		}
		else if(tokens.value[*i].type == token_op_minus_minus) {
			LOG(PRN_GRN, "tokens.value[*i].type == token_op_minus_minus");
			++*i;
			node_post_expr_t *tmp = malloc(sizeof(node_post_expr_t));
			memcpy(tmp, node, sizeof(node_post_expr_t));
			*node = (node_post_expr_t) {
				.type = node_post_expr,
				.op = op_dec,
				.post_expr_node = tmp
			};
			continue;
		}
		--*i;
		tree_offset--;
		LOG(PRN_GRN, "end");
		return node;
	}
}

node_mul_expr_t *parse_mul_expr(LIST(token_t) tokens, size_t *i) {
	LOG(PRN_GRN, "start");
	tree_offset++;
	print_offset();
	fprintf(tree, "mul_expr\n");
	node_mul_expr_t *node = malloc(sizeof(node_mul_expr_t));
	*node = (node_mul_expr_t) {
		.type = node_post_expr,
		.post_expr_node = parse_post_expr(tokens, i) 
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
				.rhs = parse_post_expr(tokens, i)
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
				.rhs = parse_post_expr(tokens, i)
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
				.rhs = parse_post_expr(tokens, i)
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

node_relat_expr_t *parse_relat_expr(LIST(token_t) tokens, size_t *i) {
	LOG(PRN_GRN, "start");
	tree_offset++;
	print_offset();
	fprintf(tree, "relat_expr\n");
	node_relat_expr_t *node = malloc(sizeof(node_relat_expr_t));
	*node = (node_relat_expr_t) {
		.type = node_add_expr,
		.add_expr_node = parse_add_expr(tokens, i)
	};
	while(1) {
		++*i;
		if(tokens.value[*i].type == token_op_greater_than) {
			++*i;
			node_relat_expr_t *tmp = malloc(sizeof(node_relat_expr_t));
			memcpy(tmp, node, sizeof(node_relat_expr_t));
			*node = (node_relat_expr_t) {
				.type = node_relat_expr,
				.op = op_g,
				.lhs = tmp,
				.rhs = parse_add_expr(tokens, i)
			};
			continue;
		}
		else if(tokens.value[*i].type == token_op_greater_than_equal_to) {
			++*i;
			node_relat_expr_t *tmp = malloc(sizeof(node_relat_expr_t));
			memcpy(tmp, node, sizeof(node_relat_expr_t));
			*node = (node_relat_expr_t) {
				.type = node_relat_expr,
				.op = op_ge,
				.lhs = tmp,
				.rhs = parse_add_expr(tokens, i)
			};
			continue;
		}
		else if(tokens.value[*i].type == token_op_less_than) {
			++*i;
			node_relat_expr_t *tmp = malloc(sizeof(node_relat_expr_t));
			memcpy(tmp, node, sizeof(node_relat_expr_t));
			*node = (node_relat_expr_t) {
				.type = node_relat_expr,
				.op = op_l,
				.lhs = tmp,
				.rhs = parse_add_expr(tokens, i)
			};
			continue;
		}
		else if(tokens.value[*i].type == token_op_less_than_equal_to) {
			++*i;
			node_relat_expr_t *tmp = malloc(sizeof(node_relat_expr_t));
			memcpy(tmp, node, sizeof(node_relat_expr_t));
			*node = (node_relat_expr_t) {
				.type = node_relat_expr,
				.op = op_le,
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

node_equal_expr_t *parse_equal_expr(LIST(token_t) tokens, size_t *i) {
	LOG(PRN_GRN, "start");
	tree_offset++;
	print_offset();
	fprintf(tree, "equal_expr\n");
	node_equal_expr_t *node = malloc(sizeof(node_equal_expr_t));
	*node = (node_equal_expr_t) {
		.type = node_relat_expr,
		.relat_expr_node = parse_relat_expr(tokens, i)
	};
	while(1) {
		++*i;
		if(tokens.value[*i].type == token_op_equals_equals) {
			++*i;
			node_equal_expr_t *tmp = malloc(sizeof(node_equal_expr_t));
			memcpy(tmp, node, sizeof(node_equal_expr_t));
			*node = (node_equal_expr_t) {
				.type = node_equal_expr,
				.op = op_equ,
				.lhs = tmp,
				.rhs = parse_relat_expr(tokens, i)
			};
			continue;
		}
		else if(tokens.value[*i].type == token_op_not_equals) {
			++*i;
			node_equal_expr_t *tmp = malloc(sizeof(node_equal_expr_t));
			memcpy(tmp, node, sizeof(node_equal_expr_t));
			*node = (node_equal_expr_t) {
				.type = node_equal_expr,
				.op = op_neq,
				.lhs = tmp,
				.rhs = parse_relat_expr(tokens, i)
			};
			continue;
		}
		tree_offset--;
		--*i;
		LOG(PRN_GRN, "end");
		return node;
	}
}

node_assign_expr_t *parse_assign_expr(LIST(token_t) tokens, size_t *i) {
	LOG(PRN_GRN, "start");
	tree_offset++;
	print_offset();
	fprintf(tree, "assign_expr\n");
	node_assign_expr_t *node = malloc(sizeof(node_assign_expr_t));
	if(tokens.value[*i + 1].type != token_op_equals) {
		node->type = node_equal_expr;
		node->equal_expr_node = parse_equal_expr(tokens, i);
		goto end;
	}
	node->type = node_assign_expr;
	node->lhs = parse_var(tokens, i);
	LOG(PRN_GRN, "set lhs");
	++*i;
	if(tokens.value[*i].type != token_op_equals) {
		LOG(PRN_GRN, "ERROR %s", tokens.value[*i].value);
		exit(1);
	}
	++*i;
	node->rhs = parse_equal_expr(tokens, i);
	LOG(PRN_GRN, "rhs set");
	++*i;

end:
	tree_offset--;
	LOG(PRN_GRN, "end");
	return node;
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

node_compound_statement_t *parse_compound_statement(LIST(token_t) tokens, size_t *i) {
	LOG(PRN_GRN, "start");
	tree_offset++;
	print_offset();
	fprintf(tree, "compound_statement\n");
	node_compound_statement_t *node = malloc(sizeof(node_compound_statement_t));
	INIT_LIST(node->block_item_nodes, 0);
	scope_t scope;
	INIT_LIST(scope.objs, curr_scope.objs.length);
	memcpy(scope.objs.value, curr_scope.objs.value, curr_scope.objs.length * sizeof(obj_t));
	LIST_APPEND(scopes, scope);

	LOG(PRN_GRN, "%s", tokens.value[*i].value);
	++*i;
	while(tokens.value[*i].type != token_op_right_curly_brace) {
		LOG(PRN_GRN, "loop %s", tokens.value[*i].value);
		LIST_APPEND(node->block_item_nodes, parse_block_item(tokens, i));
		++*i;
	}
	LOG(PRN_GRN, "loop end");
	LIST_ADD(scopes, -1); // remove scope

	tree_offset--;
	LOG(PRN_GRN, "end");
	return node;
}

node_block_item_t *parse_block_item(LIST(token_t) tokens, size_t *i) {
	LOG(PRN_GRN, "start");
	tree_offset++;
	print_offset();
	fprintf(tree, "block_item\n");

	node_block_item_t *node = malloc(sizeof(node_block_item_t));
	LOG(PRN_GRN, "%s", tokens.value[*i].value);
	switch(tokens.value[*i].type) {
	case token_keyword_int:
		*node = (node_block_item_t) {
			.type = node_declaration,
			.declaration_node = parse_declaration(tokens, i)
		};
		break;
	default:
		*node = (node_block_item_t) {
			.type = node_statement,
			.statement_node = parse_statement(tokens, i)
		};
		break;
	}
	
	tree_offset--;
	LOG(PRN_GRN, "end");
	return node;
}

node_if_t *parse_if(LIST(token_t) tokens, size_t *i) {
	LOG(PRN_GRN, "start");
	tree_offset++;
	print_offset();
	fprintf(tree, "if\n");
	node_if_t *node = malloc(sizeof(node_if_t));
	++*i;
	if(tokens.value[*i].type != token_op_left_paren) {
		LOG(PRN_GRN, "ERROR");
		exit(1);
	}
	++*i;
	node->expr_node = parse_expr(tokens, i);
	++*i;
	if(tokens.value[*i].type != token_op_right_paren) {
		LOG(PRN_GRN, "ERROR");
		exit(1);
	}
	++*i;
	node->if_branch = parse_statement(tokens, i);
	++*i;
	if(tokens.value[*i].type == token_keyword_else) {
		++*i;
		LOG(PRN_GRN, "detected else");
		node->type = node_if_else;
		node->else_branch = parse_statement(tokens, i);
		goto end;
	}
	--*i;
	LOG(PRN_GRN, "no else");
	node->type = node_if;
	
end:
	tree_offset--;
	LOG(PRN_GRN, "end");
	return node;
}

node_while_t *parse_while(LIST(token_t) tokens, size_t *i) {
	LOG(PRN_GRN, "start");
	tree_offset++;
	print_offset();
	node_while_t *node = malloc(sizeof(node_while_t));
	if(tokens.value[*i].type == token_keyword_while) {
		LOG(PRN_GRN, "while");
		fprintf(tree, "while\n");
		node->type = node_while;
		++*i;
		if(tokens.value[*i].type != token_op_left_paren) {
			LOG(PRN_GRN, "ERROR %s", tokens.value[*i].value);
			exit(1);
		}
		++*i;
		node->expr_node = parse_expr(tokens, i);
		++*i;
		if(tokens.value[*i].type != token_op_right_paren) {
			LOG(PRN_GRN, "ERROR %s", tokens.value[*i].value);
			exit(1);
		}
		++*i;
		node->body = parse_statement(tokens, i);
	}
	else {
		LOG(PRN_GRN, "do_while");
		fprintf(tree, "do_while\n");
		node->type = node_do_while;
		++*i;
		node->body = parse_statement(tokens, i);
		++*i;
		if(tokens.value[*i].type != token_keyword_while) {
			LOG(PRN_GRN, "ERROR %s", tokens.value[*i].value);
			exit(1);
		}
		++*i;
		if(tokens.value[*i].type != token_op_left_paren) {
			LOG(PRN_GRN, "ERROR %s", tokens.value[*i].value);
			exit(1);
		}
		++*i;
		node->expr_node = parse_expr(tokens, i);
		++*i;
		if(tokens.value[*i].type != token_op_right_paren) {
			LOG(PRN_GRN, "ERROR %s", tokens.value[*i].value);
			exit(1);
		}
		++*i;
		if(tokens.value[*i].type != token_op_semicolon) {
			LOG(PRN_GRN, "ERROR %s", tokens.value[*i].value);
			exit(1);
		}
	}
	tree_offset--;
	LOG(PRN_GRN, "end");
	return node;
}

node_for_t *parse_for(LIST(token_t) tokens, size_t *i) {
	LOG(PRN_GRN, "start");
	tree_offset++;
	print_offset();
	fprintf(tree, "for\n");
	node_for_t *node = malloc(sizeof(node_for_t));
	++*i;
	if(tokens.value[*i].type != token_op_left_paren) {
		LOG(PRN_GRN, "ERROR %s", tokens.value[*i].value);
		exit(1);
	}
	++*i;
	if(tokens.value[*i].type == token_keyword_int) {
		node->type = node_decl_for;
		node->declaration_node = parse_declaration(tokens, i);
	}
	else {
		node->type = node_for;
		node->expr1 = parse_expr(tokens, i);
		++*i;
		if(tokens.value[*i].type != token_op_semicolon) {
			LOG(PRN_GRN, "ERROR %s", tokens.value[*i].value);
			exit(1);
		}
	}
	++*i;
	node->expr2 = parse_expr(tokens, i);
	++*i;
	if(tokens.value[*i].type != token_op_semicolon) {
		LOG(PRN_GRN, "ERROR %s", tokens.value[*i].value);
		exit(1);
	}
	++*i;
	node->expr3 = parse_expr(tokens, i);
	if(tokens.value[*i].type != token_op_right_paren) {
		LOG(PRN_GRN, "ERROR %s", tokens.value[*i].value);
		exit(1);
	}
	++*i;
	node->body = parse_statement(tokens, i);

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
	case token_op_left_curly_brace:
		LOG(PRN_GRN, "detected op_left_curly_brace");
		*node = (node_statement_t) {
			.type = node_compound_statement,
			.compound_statement_node = parse_compound_statement(tokens, i)
		};
		tree_offset--;
		LOG(PRN_GRN, "end left curly");
		return node;
	case token_keyword_if:
		LOG(PRN_GRN, "detected keyword if");
		*node = (node_statement_t) {
			.type = node_if,
			.if_node = parse_if(tokens, i)
		};
		tree_offset--;
		LOG(PRN_GRN, "end if");
		return node;
	case token_keyword_while:
		LOG(PRN_GRN, "detected keyword while");
		*node = (node_statement_t) {
			.type = node_while,
			.while_node = parse_while(tokens, i)
		};
		tree_offset--;
		LOG(PRN_GRN, "end while");
		return node;
	case token_keyword_do:
		LOG(PRN_GRN, "detected keyword do");
		*node = (node_statement_t) {
			.type = node_while,
			.while_node = parse_while(tokens, i)
		};
		tree_offset--;
		LOG(PRN_GRN, "end do");
		return node;
	case token_keyword_for:
		LOG(PRN_GRN, "detected keyword for");
		*node = (node_statement_t) {
			.type = node_for,
			.for_node = parse_for(tokens, i)
		};
		tree_offset--;
		LOG(PRN_GRN, "end for");
		return node;
	case token_identifier:
		LOG(PRN_GRN, "token_identifier: %s", tokens.value[*i].value);
		if(identifier_is_var(tokens.value[*i])) {
			LOG(PRN_GRN, "identifier is var");
			*node = (node_statement_t) {
				.type = node_expr,
				.expr_node = parse_expr(tokens, i)
			};
			tree_offset--;
			return node;
		}
		else if(identifier_is_fn(tokens.value[*i])) {
			LOG(PRN_GRN, "identifier is fn");
			*node = (node_statement_t) {
				.type = node_expr,
				.expr_node = parse_expr(tokens, i)
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

node_declaration_t *parse_declaration(LIST(token_t) tokens, size_t *i) {
	LOG(PRN_GRN, "start");
	tree_offset++;
	print_offset();
	fprintf(tree, "declaration\n");

	node_declaration_t *node = malloc(sizeof(node_declaration_t));
	expect(token_keyword_int);
	LOG(PRN_GRN, "%s", tokens.value[*i].value);
	if(tokens.value[*i].type != token_identifier) {
		LOG(PRN_GRN, "tokens.value[*i].type != token_identifier");
		*node = (node_declaration_t) {
			.has_init_declarator = false
		};
	}
	else {
		LOG(PRN_GRN, "tokens.value[*i].type == token_identifier");
		stack_size += 4;
		*node = (node_declaration_t) {
			.has_init_declarator = true,
			.stack_offset = stack_size,
			.init_declarator_node = parse_init_declarator(tokens, i)
		};
		obj_t var = (obj_t) {
			.is_fn = false,
			.stack_offset = stack_size,
			.token = node->init_declarator_node->token
		};
		LOG(PRN_GRN, "%s", var.token.value);
		LIST_APPEND(curr_scope.objs, var);
	}
	++*i;
	if(tokens.value[*i].type != token_op_semicolon) {
		LOG(PRN_GRN, "ERROR: %s", tokens.value[*i].value);
	}

	tree_offset--;
	LOG(PRN_GRN, "end");
	return node;
}

node_init_declarator_t *parse_init_declarator(LIST(token_t) tokens, size_t *i) {
	LOG(PRN_GRN, "start");
	tree_offset++;
	print_offset();
	fprintf(tree, "init_declarator\n");

	node_init_declarator_t *node = malloc(sizeof(node_init_declarator_t));
	expect(token_identifier);
	if(tokens.value[*i].type != token_op_equals) {
		*node = (node_init_declarator_t) {
			.has_initializer = false,
			.token = tokens.value[*i - 1]
		};
	}
	else {
		++*i;
		*node = (node_init_declarator_t) {
			.has_initializer = true,
			.token = tokens.value[*i - 2],
			.initializer_node = parse_assign_expr(tokens, i)
		};
	}
	
	tree_offset--;
	LOG(PRN_GRN, "end");
	return node;
}

bool identifier_is_var(token_t token) {
	LOG(PRN_GRN, "start");
	for(size_t i = 0; i < curr_scope.objs.length; i++) {
		LOG(PRN_GRN, "loop: %s", curr_scope.objs.value[i].token.value);
		if(!strcmp(token.value, curr_scope.objs.value[i].token.value) &&
		   curr_scope.objs.value[i].is_fn == false) {
			LOG(PRN_GRN, "identifier %s is var", token.value);
			LOG(PRN_GRN, "end");
			return true;
		}
	}
	LOG(PRN_GRN, "identifier %s is not var", token.value);
	LOG(PRN_GRN, "end");
	return false;
}

bool identifier_is_fn(token_t token) {
	LOG(PRN_GRN, "start");
	for(size_t i = 0; i < curr_scope.objs.length; i++) {
		if(!strcmp(token.value, curr_scope.objs.value[i].token.value) &&
		   curr_scope.objs.value[i].is_fn == true) {
			LOG(PRN_GRN, "identifier %s is fn", token.value);
			LOG(PRN_GRN, "end");
			return true;
		}
	}
	LOG(PRN_GRN, "identifier %s is not fn", token.value);
	LOG(PRN_GRN, "end");
	return false;
}
