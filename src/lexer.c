#include "lexer.h"
#include "util.h"

u64 line_number = 1;

const char *token_table[] = {
	"auto",
	"break",
	"case",
	"char",
	"const",
	"continue",
	"default",
	"do",
	"double",
	"else",
	"enum",
	"extern",
	"float",
	"for",
	"goto",
	"if",
	"int",
	"long",
	"register",
	"return",
	"short",
	"signed",
	"sizeof",
	"static",
	"struct",
	"switch",
	"typedef",
	"union",
	"unsigned",
	"void",
	"volatile",
	"while",
	"inline",
	"restrict",
	"bool",
	"complex",
	"alignas",
	"alignof",
	"atomic",
	"generic",
	"constexpr",
	"true",
	"false",
	"nullptr",
	"static_assert",
	"thread_local",
	"typeof",
	"typeof_unqual",
	"bitint",
	"decimal32",
	"decimal64",
	"decimal128",
	NULL, // token_identifier
	NULL, // token_int_literal
	NULL, // token_unsigned_int_literal
	NULL, // token_long_literal
	NULL, // token_unsigned_long_literal
	NULL, // token_long_long_literal
	NULL, // token_unsigned_long_long_literal
	NULL, // token_float_literal
	NULL, // token_double_literal
	NULL, // token_char_literal
	NULL, // token_string_literal
	"+",
	"-",
	"*",
	"/",
	"%",
	"=",
	"+=",
	"-=",
	"*=",
	"/=",
	"%=",
	"&=",
	"|=",
	"^=",
	"<<=",
	">>=",
	"~",
	"&",
	"|",
	"^",
	"<<",
	">>",
	"!",
	"&&",
	"||",
	"?",
	":",
	"==",
	"!=",
	"(",
	")",
	"++",
	"--",
	".",
	"->",
	"<",
	"<=",
	">",
	">=",
	"[",
	"]",
	"[[",
	"]]",
	",",
	";",
	"{",
	"}"
};


token_t tokenize_identifier(char *str, u64 start, u64 *i) {
	while(isalnum(str[*i]) || str[*i] == '_') {
		// c operator precedence in a nutshell
		++*i;
	}
	char *buffer = substr(str, start, *i);
	for(int j = 0; j < token_type_count - 1; j++) {
		if(!token_table[j]) {
			break;
		}
		if(strcmp(buffer, token_table[j]) == 0) {
			return (token_t) {
				.type = j,
				.value = buffer,
			};
		}
	}
	return (token_t) {
		.type = token_identifier,
		.value = buffer,
	};
}

token_t tokenize_int_literal(char *str, u64 start, u64 *i) {
	while(isdigit(str[*i])) {
		++*i;
	}
	return (token_t) {
		.type = token_int_literal,
		.value = substr(str, start, *i),
	};
}

token_t tokenize_char_literal(char *str, u64 start, u64 *i) {
	if(str[*i] == '\\') {
		printf("\\ detected\n");
		++*i;
	}
	printf("292 %c\n", str[*i]);
	++*i;
	printf("294 %c\n", str[*i]);
	if(str[*i] != '\'') {
		printf("\nerror invalid char lit\n");
		exit(1);
	}
	return (token_t) {
		.type = token_char_literal,
		.value = substr(str, start, *i)
	};
}

token_t tokenize_operator(char *str, u64 start, u64 *i) {
	switch(str[start]) {
	case '+':
		if(str[*i] == '=') {
			++*i;
			return (token_t) {
				.type = token_op_plus_equals,
				.value = substr(str, start, *i),
			};
		}
		else if(str[*i] == '+') {
			++*i;
			return (token_t) {
				.type = token_op_plus_plus,
				.value = substr(str, start, *i),
			};
		}
		return (token_t) {
			.type = token_op_plus,
			.value = substr(str, start, *i),
		};
		break;
	case '-':
		if(str[*i] == '=') {
			++*i;
			return (token_t) {
				.type = token_op_minus_equals,
				.value = substr(str, start, *i),
			};
		}
		else if(str[*i] == '-') {
			++*i;
			return (token_t) {
				.type = token_op_minus_minus,
				.value = substr(str, start, *i),
			};
		}
		else if(str[*i] == '>') {
			++*i;
			return (token_t) {
				.type = token_op_arrow,
				.value = substr(str, start, *i),
			};
		}
		return (token_t) {
			.type = token_op_minus,
			.value = substr(str, start, *i),
		};
		break;
	case '*':
		if(str[*i] == '=') {
			++*i;
			return (token_t) {
				.type = token_op_asterisk_equals,
				.value = substr(str, start, *i),
			};
		}
		return (token_t) {
			.type = token_op_asterisk,
			.value = substr(str, start, *i),
		};
		break;
	case '/':
		if(str[*i] == '=') {
			++*i;
			return (token_t) {
				.type = token_op_slash_equals,
				.value = substr(str, start, *i),
			};
		}
		return (token_t) {
			.type = token_op_slash,
			.value = substr(str, start, *i),
		};
		break;
	case '%':
		if(str[*i] == '=') {
			++*i;
			return (token_t) {
				.type = token_op_percent_equals,
				.value = substr(str, start, *i),
			};
		}
		return (token_t) {
			.type = token_op_percent,
			.value = substr(str, start, *i),
		};
		break;
	case '&':
		if(str[*i] == '=') {
			++*i;
			return (token_t) {
				.type = token_op_bitwise_and_equals,
				.value = substr(str, start, *i),
			};
		}
		else if(str[*i] == '&') {
			++*i;
			return (token_t) {
				.type = token_op_boolean_and,
				.value = substr(str, start, *i),
			};
		}
		return (token_t) {
			.type = token_op_bitwise_and,
			.value = substr(str, start, *i),
		};
		break;
	case '|':
		if(str[*i] == '=') {
			++*i;
			return (token_t) {
				.type = token_op_bitwise_or_equals,
				.value = substr(str, start, *i),
			};
		}
		else if(str[*i] == '|') {
			++*i;
			return (token_t) {
				.type = token_op_boolean_or,
				.value = substr(str, start, *i),
			};
		}
		return (token_t) {
			.type = token_op_bitwise_or,
			.value = substr(str, start, *i),
		};
		break;
	case '^':
		if(str[*i] == '=') {
			++*i;
			return (token_t) {
				.type = token_op_bitwise_xor_equals,
				.value = substr(str, start, *i),
			};
		}
		return (token_t) {
			.type = token_op_bitwise_xor,
			.value = substr(str, start, *i),
		};
		break;
	case '<':
		if(str[*i] == '=') {
			++*i;
			return (token_t) {
				.type = token_op_less_than_equal_to,
				.value = substr(str, start, *i),
			};
		}
		else if(str[*i] == '<' && str[*i + 1] == '=') {
			*i += 2;
			return (token_t) {
				.type = token_op_bitshift_left_equals,
				.value = substr(str, start, *i),
			};
		}
		else if(str[*i] == '<') {
			++*i;
			return (token_t) {
				.type = token_op_bitshift_left,
				.value = substr(str, start, *i),
			};
		}
		return (token_t) {
			.type = token_op_less_than,
			.value = substr(str, start, *i),
		};
		break;
	case '>':
		if(str[*i] == '=') {
			++*i;
			return (token_t) {
				.type = token_op_greater_than_equal_to,
				.value = substr(str, start, *i),
			};
		}
		else if(str[*i] == '>' && str[*i + 1] == '=') {
			*i += 2;
			return (token_t) {
				.type = token_op_bitshift_right_equals,
				.value = substr(str, start, *i),
			};
		}
		else if(str[*i] == '>') {
			++*i;
			return (token_t) {
				.type = token_op_bitshift_right,
				.value = substr(str, start, *i),
			};
		}
		return (token_t) {
			.type = token_op_greater_than,
			.value = substr(str, start, *i),
		};
		break;
	case '!':
		if(str[*i] == '=') {
			++*i;
			return (token_t) {
				.type = token_op_not_equals,
				.value = substr(str, start, *i),
			};
		}
		return (token_t) {
			.type = token_op_boolean_not,
			.value = substr(str, start, *i),
		};
		break;
	case '=':
		if(str[*i] == '=') {
			++*i;
			return (token_t) {
				.type = token_op_equals_equals,
				.value = substr(str, start, *i),
			};
		}
		return (token_t) {
			.type = token_op_equals,
			.value = substr(str, start, *i),
		};
		break;
	case '[':
		if(str[*i] == '[') {
			++*i;
			return (token_t) {
				.type = token_op_attr_begin,
				.value = substr(str, start, *i),
			};
		}
		return (token_t) {
			.type = token_op_left_bracket,
			.value = substr(str, start, *i),
		};
		break;
	case ']':
		if(str[*i] == ']') {
			++*i;
			return (token_t) {
				.type = token_op_attr_begin,
				.value = substr(str, start, *i),
			};
		}
		return (token_t) {
			.type = token_op_left_bracket,
			.value = substr(str, start, *i),
		};
		break;
	case ';':
		return (token_t) {
			.type = token_op_semicolon,
			.value = substr(str, start, *i),
		};
		break;
	case '{':
		return (token_t) {
			.type = token_op_right_curly_brace,
			.value = substr(str, start, *i),
		};
		break;
	case '}':
		return (token_t) {
			.type = token_op_left_curly_brace,
			.value = substr(str, start, *i),
		};
		break;
	default:
		char *buffer = substr(str, start, *i);
		for(int j = token_op_plus; j < token_type_count; j++) {
			if(strcmp(buffer, token_table[j]) == 0) {
				return (token_t) {
					.value = buffer,
					.type = j,
				};
			}
		}
		error(error_invalid_op, "invalid operator");
	}
	error(error_invalid_op, "invalid operator");
}

LIST(token_t) tokenize(char *str) {
	LOG(PRN_RED, "called tokenize()");
	LIST(token_t) tokens;
	INIT_LIST(tokens, 0);
	for(u64 i = 0; str[i]; i++) {
		LOG(PRN_RED, "loop");
		// identifiers/keywords
		if(isalpha(str[i]) || str[i] == '_') {
			LOG(PRN_RED, "detected identifier");
			u64 start = i;
			i++;
			LIST_APPEND(tokens, tokenize_identifier(str, start, &i));
			tokens.value[i].line_num = line_number;
			i--;
		}
		else if(str[i] == '\n') {
			LOG(PRN_RED, "detected newline");
			line_number++;
			continue;
		}
		// whitespace
		else if(isspace(str[i])) {
			LOG(PRN_RED, "detected whitespace");
			continue;
		}
		// integer literals
		else if(isdigit(str[i])) {
			LOG(PRN_RED, "detected int lit");
			u64 start = i;
			i++;
			LIST_APPEND(tokens, tokenize_int_literal(str, start, &i));
			tokens.value[i].line_num = line_number;
			i--;
		}
		// operators
		else {
			LOG(PRN_RED, "detected operator");
			u64 start = i;
			i++;
			LIST_APPEND(tokens, tokenize_operator(str, start, &i));
			tokens.value[i].line_num = line_number;
			i--;
		}
	}
	LOG(PRN_RED, "end loop");
	return tokens;
}
