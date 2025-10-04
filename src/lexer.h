#ifndef LEXER_H
#define LEXER_H

#include <string.h>
#include <ctype.h>
#include "ds.h"
#include "util.h"

typedef enum token_type {
	token_keyword_auto = 0,
	token_keyword_break,
	token_keyword_case,
	token_keyword_char,
	token_keyword_const,
	token_keyword_continue,
	token_keyword_default,
	token_keyword_do,
	token_keyword_double,
	token_keyword_else,
	token_keyword_enum,
	token_keyword_extern,
	token_keyword_float,
	token_keyword_for,
	token_keyword_goto,
	token_keyword_if,
	token_keyword_int,
	token_keyword_long,
	token_keyword_register,
	token_keyword_return,
	token_keyword_short,
	token_keyword_signed,
	token_keyword_sizeof,
	token_keyword_static,
	token_keyword_struct,
	token_keyword_switch,
	token_keyword_typedef,
	token_keyword_union,
	token_keyword_unsigned,
	token_keyword_void,
	token_keyword_volatile,
	token_keyword_while,
	token_keyword_inline,
	token_keyword_restrict,
	token_keyword_bool,
	token_keyword_complex,
	token_keyword_alignas,
	token_keyword_alignof,
	token_keyword_atomic,
	token_keyword_generic,
	token_keyword_constexpr,
	token_keyword_true,
	token_keyword_false,
	token_keyword_nullptr,
	token_keyword_static_assert,
	token_keyword_thread_local,
	token_keyword_typeof,
	token_keyword_typeof_unqual,
	token_keyword_bitint,
	token_keyword_decimal32,
	token_keyword_decimal64,
	token_keyword_decimal128,
	token_identifier,
	token_int_literal,
	token_unsigned_int_literal,
	token_long_literal,
	token_unsigned_long_literal,
	token_long_long_literal,
	token_unsigned_long_long_literal,
	token_float_literal,
	token_double_literal,
	token_char_literal,
	token_string_literal,
	token_op_plus,
	token_op_minus,
	token_op_asterisk,
	token_op_slash,
	token_op_percent,
	token_op_equals,
	token_op_plus_equals,
	token_op_minus_equals,
	token_op_asterisk_equals,
	token_op_slash_equals,
	token_op_percent_equals,
	token_op_bitwise_and_equals,
	token_op_bitwise_or_equals,
	token_op_bitwise_xor_equals,
	token_op_bitshift_left_equals,
	token_op_bitshift_right_equals,
	token_op_bitwise_not,
	token_op_bitwise_and,
	token_op_bitwise_or,
	token_op_bitwise_xor,
	token_op_bitshift_left,
	token_op_bitshift_right,
	token_op_boolean_not,
	token_op_boolean_and,
	token_op_boolean_or,
	token_op_question_mark,
	token_op_colon,
	token_op_equals_equals,
	token_op_not_equals,
	token_op_left_paren,
	token_op_right_paren,
	token_op_plus_plus,
	token_op_minus_minus,
	token_op_period,
	token_op_arrow,
	token_op_less_than,
	token_op_less_than_equal_to,
	token_op_greater_than,
	token_op_greater_than_equal_to,
	token_op_left_bracket,
	token_op_right_bracket,
	token_op_attr_begin,
	token_op_attr_end,
	token_op_comma,
	token_op_semicolon,
	token_op_left_curly_brace,
	token_op_right_curly_brace,

	token_type_count
} token_type;

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

typedef struct token_t {
	token_type type;
	char *value;
} token_t;

NEW_LIST(token_t);

token_t tokenize_identifier(char *str, u64 start, u64 *i);
token_t tokenize_int_literal(char *str, u64 start, u64 *i);
token_t tokenize_operator(char *str, u64 start, u64 *i);
LIST(token_t) tokenize(char *str);

#endif // LEXER_H

#ifdef LEXER_IMPL

token_t tokenize_identifier(char *str, u64 start, u64 *i) {
	token_t token;
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
			token = (token_t) {
				.type = j,
				.value = buffer,
			};
			goto found;
		}
	}
	token = (token_t) {
		.type = token_identifier,
		.value = buffer,
	};
found:
	return token; 
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
				goto found;
			}
		}
		error(invalid_op, "invalid operator");
	}
	error(invalid_op, "invalid operator");
found:
}

LIST(token_t) tokenize(char *str) {
	printf("Tokenize Arg: %s\n", str);
	LIST(token_t) tokens;
	INIT_LIST(tokens, 0);
	for(u64 i = 0; str[i]; i++) {
		// identifiers/keywords
		if(isalpha(str[i]) || str[i] == '_') {
			u64 start = i;
			i++;
			LIST_APPEND(tokens, tokenize_identifier(str, start, &i));
			i--;
		}
		// whitespace
		else if(isspace(str[i])) {
			continue;
		}
		// integer literals
		else if(isdigit(str[i])) {
			u64 start = i;
			i++;
			LIST_APPEND(tokens, tokenize_int_literal(str, start, &i));
			i--;
		}
		// operators
		else {
			u64 start = i;
			i++;
			LIST_APPEND(tokens, tokenize_operator(str, start, &i));
			i--;
		}
	}
	return tokens;
}

#endif // LEXER_IMPL
