#ifndef LEXER_H
#define LEXER_H

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "util.h"

typedef enum token_type {
    cee_int_literal = 0,
    cee_semicolon,
    cee_identifier,
    cee_left_curly_brace,
    cee_right_curly_brace,
    cee_OP_plus,
    cee_OP_minus,
    cee_OP_asterisk,
    cee_OP_slash,
    cee_OP_percent,
    cee_OP_assignment_equals,
    cee_OP_assignment_add,
    cee_OP_assignment_subtract,
    cee_OP_assignment_multiply,
    cee_OP_assignment_divide,
    cee_OP_assignment_modulus,
    cee_OP_assignment_bitwise_AND,
    cee_OP_assignment_bitwise_OR,
    cee_OP_assignment_bitwise_XOR,
    cee_OP_assignment_bitshift_left,
    cee_OP_assignment_bitshift_right,
    cee_OP_bitwise_NOT,
    cee_OP_ampersand,
    cee_OP_bitwise_OR,
    cee_OP_bitwise_XOR,
    cee_OP_bitshift_left,
    cee_OP_bitshift_right,
    cee_OP_boolean_NOT,
    cee_OP_boolean_AND,
    cee_OP_boolean_OR,
    cee_OP_question_mark,
    cee_OP_colon,
    cee_OP_equals,
    cee_OP_NOT_equals,
    cee_OP_left_parentheses,
    cee_OP_right_parentheses,
    cee_OP_increment,
    cee_OP_decrement,
    cee_OP_member_select,
    cee_OP_pointer_member_select,
    cee_OP_less_than,
    cee_OP_less_than_equal_to,
    cee_OP_greater_than,
    cee_OP_greater_than_equal_to,
    cee_OP_left_bracket,
    cee_OP_right_bracket,
    cee_OP_comma,
    cee_auto,
    cee_break,
    cee_case,
    cee_char,
    cee_const,
    cee_continue,
    cee_default,
    cee_do,
    cee_double,
    cee_else,
    cee_enum,
    cee_extern,
    cee_float,
    cee_for,
    cee_goto,
    cee_if,
    cee_int,
    cee_long,
    cee_register,
    cee_return,
    cee_short,
    cee_signed,
    cee_sizeof,
    cee_static,
    cee_struct,
    cee_switch,
    cee_typedef,
    cee_union,
    cee_unsigned,
    cee_void,
    cee_volatile,
    cee_while,
    cee_inline,
    cee_restrict,
    cee__Complex,
    cee__Imaginary,
    cee__Atomic,
    cee__Generic,
    cee__Noreturn,
    cee_alignas,
    cee_alignof,
    cee_bool,
    cee_constexpr,
    cee_false,
    cee_nullptr,
    cee_static_assert,
    cee_thread_local,
    cee_true,
    cee_typeof,
    cee_typeof_unqual,
    cee__BitInt,
    cee__Decimal32,
    cee__Decimal64,
    cee__Decimal128,

    cee_keyword_count
} token_type;

static const char *keyword_table[] = {
    NULL, // cee_int_literal
    NULL, // cee_semicolon
    NULL, // cee_identifier
    NULL, // cee_left_curly_brace
    NULL, // cee_right_curly_brace
    NULL, // cee_OP_plus
    NULL, // cee_OP_minus
    NULL, // cee_OP_asterisk
    NULL, // cee_OP_slash
    NULL, // cee_OP_percent
    NULL, // cee_OP_assignment_equals
    NULL, // cee_OP_assignment_add
    NULL, // cee_OP_assignment_subtract
    NULL, // cee_OP_assignment_multiply
    NULL, // cee_OP_assignment_divide
    NULL, // cee_OP_assignment_modulus
    NULL, // cee_OP_assignment_bitwise_AND
    NULL, // cee_OP_assignment_bitwise_OR
    NULL, // cee_OP_assignment_bitwise_XOR
    NULL, // cee_OP_assignment_bitshift_left
    NULL, // cee_OP_assignment_bitshift_right
    NULL, // cee_OP_bitwise_NOT
    NULL, // cee_OP_ampersand
    NULL, // cee_OP_bitwise_OR
    NULL, // cee_OP_bitwise_XOR
    NULL, // cee_OP_bitshift_left
    NULL, // cee_OP_bitshift_right
    NULL, // cee_OP_boolean_NOT
    NULL, // cee_OP_boolean_AND
    NULL, // cee_OP_boolean_OR
    NULL, // cee_OP_question_mark
    NULL, // cee_OP_colon
    NULL, // cee_OP_equals
    NULL, // cee_OP_NOT_equals
    NULL, // cee_OP_left_parentheses
    NULL, // cee_OP_right_parentheses
    NULL, // cee_OP_increment
    NULL, // cee_OP_decrement
    NULL, // cee_OP_member_select
    NULL, // cee_OP_pointer_member_select
    NULL, // cee_OP_less_than
    NULL, // cee_OP_less_than_equal_to
    NULL, // cee_OP_greater_than
    NULL, // cee_OP_greater_than_equal_to
    NULL, // cee_OP_left_bracket
    NULL, // cee_OP_right_bracket
    NULL, // cee_OP_comma
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
    "_Complex",
    "_Imaginary",
    "_Atomic",
    "_Generic",
    "_Noreturn",
    "_Alignas",
    "_Alignof",
    "_Bool",
    "constexpr",
    "false",
    "nullptr",
    "_Static_assert",
    "_Thread_local",
    "true",
    "typeof",
    "typeof_unqual",
    "_BitInt",
    "_Decimal32",
    "_Decimal64",
    "_Decimal128",
};

extern int token_amount;

typedef struct {
    token_type type;
    char *value;
} token_t;

token_t *tokenize(char*);
token_t *token_push_back(token_t*, token_type, char*, size_t);

#endif // LEXER_H
