#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum {
    cee_int_literal,
    cee_semicolon,
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
    cee__Decimal128
} token_type;

typedef struct {
    token_type type;
    char *value;
} token_t;

char *read_whole_file(FILE *);
token_t *tokenize(char *);
char *char_push_back(char *, char, size_t);
token_t *token_push_back(token_t *, token_type, char *, size_t);

int token_amount = 0;

int main(int argc, char **argv) {
    if(argc == 1) {
        printf("skill issue: you gotta put a file");
        exit(1);
    }
    FILE *file = fopen(argv[1], "r");
    char *str = read_whole_file(file);
    token_t *tokens = tokenize(str);
    for(int i = 0; i < token_amount; i++) {
        printf("%s\n", tokens[i].value);
    }
    for(int i = 0; i < token_amount; i++) {
        free(tokens[i].value);
    }

    return 0;
}

char *read_whole_file(FILE *f) {
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);
	char *str = malloc(fsize + 1);
	fread(str, fsize, 1, f);
	str[fsize] = 0;

	return str;
}

token_t *tokenize(char *str) {
    char *buffer = (char *)malloc(0);
    token_t *tokens = (token_t *)malloc(0);
    /* loops through the whole code */
    for(int i = 0; str[i] != '\0'; i++) {
        /* anything that isnt a number must start with a letter or an underscore */
        if(isalpha(str[i]) || str[i] == '_') {
            int length = 1;
            int pos = i;
            /* increase number of letters */
            buffer = char_push_back(buffer, str[i], i + 1);
            i++;
            /* but, they can have numbers elsewhere */
            while(isalnum(str[i]) || str[i] == '_') {
                buffer = char_push_back(buffer, str[i], i + 1);
                length++;
                i++;
            }
            /* increase number of tokens */
            token_amount++;
            /* test for keyword */
            char *substring = (char *)malloc(length);
            memcpy(substring, buffer + pos, length);
            i--;
            if(strcmp(substring, "auto") == 0) {
                tokens = token_push_back(tokens, cee_auto, substring, token_amount);
            }
            else if(strcmp(substring, "break") == 0) {
                tokens = token_push_back(tokens, cee_break, substring, token_amount);
            }
            else if(strcmp(substring, "case") == 0) {
                tokens = token_push_back(tokens, cee_case, substring, token_amount);
            }
            else if(strcmp(substring, "char") == 0) {
                tokens = token_push_back(tokens, cee_char, substring, token_amount);
            }
            else if(strcmp(substring, "const") == 0) {
                tokens = token_push_back(tokens, cee_const, substring, token_amount);
            }
            else if(strcmp(substring, "continue") == 0) {
                tokens = token_push_back(tokens, cee_continue, substring, token_amount);
            }
            else if(strcmp(substring, "default") == 0) {
                tokens = token_push_back(tokens, cee_default, substring, token_amount);
            }
            else if(strcmp(substring, "do") == 0) {
                tokens = token_push_back(tokens, cee_do, substring, token_amount);
            }
            else if(strcmp(substring, "double") == 0) {
                tokens = token_push_back(tokens, cee_double, substring, token_amount);
            }
            else if(strcmp(substring, "else") == 0) {
                tokens = token_push_back(tokens, cee_else, substring, token_amount);
            }
            else if(strcmp(substring, "enum") == 0) {
                tokens = token_push_back(tokens, cee_enum, substring, token_amount);
            }
            else if(strcmp(substring, "extern") == 0) {
                tokens = token_push_back(tokens, cee_extern, substring, token_amount);
            }
            else if(strcmp(substring, "float") == 0) {
                tokens = token_push_back(tokens, cee_float, substring, token_amount);
            }
            else if(strcmp(substring, "for") == 0) {
                tokens = token_push_back(tokens, cee_for, substring, token_amount);
            }
            else if(strcmp(substring, "goto") == 0) {
                tokens = token_push_back(tokens, cee_goto, substring, token_amount);
            }
            else if(strcmp(substring, "if") == 0) {
                tokens = token_push_back(tokens, cee_if, substring, token_amount);
            }
            else if(strcmp(substring, "int") == 0) {
                tokens = token_push_back(tokens, cee_int, substring, token_amount);
            }
            else if(strcmp(substring, "long") == 0) {
                tokens = token_push_back(tokens, cee_long, substring, token_amount);
            }
            else if(strcmp(substring, "register") == 0) {
                tokens = token_push_back(tokens, cee_register, substring, token_amount);
            }
            else if(strcmp(substring, "return") == 0) {
                tokens = token_push_back(tokens, cee_return, substring, token_amount);
            }
            else if(strcmp(substring, "short") == 0) {
                tokens = token_push_back(tokens, cee_short, substring, token_amount);
            }
            else if(strcmp(substring, "signed") == 0) {
                tokens = token_push_back(tokens, cee_signed, substring, token_amount);
            }
            else if(strcmp(substring, "sizeof") == 0) {
                tokens = token_push_back(tokens, cee_sizeof, substring, token_amount);
            }
            else if(strcmp(substring, "static") == 0) {
                tokens = token_push_back(tokens, cee_static, substring, token_amount);
            }
            else if(strcmp(substring, "struct") == 0) {
                tokens = token_push_back(tokens, cee_struct, substring, token_amount);
            }
            else if(strcmp(substring, "switch") == 0) {
                tokens = token_push_back(tokens, cee_switch, substring, token_amount);
            }
            else if(strcmp(substring, "typedef") == 0) {
                tokens = token_push_back(tokens, cee_typedef, substring, token_amount);
            }
            else if(strcmp(substring, "union") == 0) {
                tokens = token_push_back(tokens, cee_union, substring, token_amount);
            }
            else if(strcmp(substring, "unsigned") == 0) {
                tokens = token_push_back(tokens, cee_unsigned, substring, token_amount);
            }
            else if(strcmp(substring, "void") == 0) {
                tokens = token_push_back(tokens, cee_void, substring, token_amount);
            }
            else if(strcmp(substring, "volatile") == 0) {
                tokens = token_push_back(tokens, cee_volatile, substring, token_amount);
            }
            else if(strcmp(substring, "while") == 0) {
                tokens = token_push_back(tokens, cee_while, substring, token_amount);
            }
            else if(strcmp(substring, "inline") == 0) {
                tokens = token_push_back(tokens, cee_inline, substring, token_amount);
            }
            else if(strcmp(substring, "restrict") == 0) {
                tokens = token_push_back(tokens, cee_restrict, substring, token_amount);
            }
            else if(strcmp(substring, "_Bool") == 0 || strcmp(substring, "bool") == 0) {
                tokens = token_push_back(tokens, cee_bool, substring, token_amount);
            }
            else if(strcmp(substring, "_Complex") == 0) {
                tokens = token_push_back(tokens, cee__Complex, substring, token_amount);
            }
            else if(strcmp(substring, "_Imaginary") == 0) {
                tokens = token_push_back(tokens, cee__Imaginary, substring, token_amount);
            }
            else if(strcmp(substring, "_Alignas") == 0 || strcmp(substring, "alignas") == 0) {
                tokens = token_push_back(tokens, cee_alignas, substring, token_amount);
            }
            else if(strcmp(substring, "_Alignof") == 0 || strcmp(substring, "alignof") == 0) {
                tokens = token_push_back(tokens, cee_alignof, substring, token_amount);
            }
            else if(strcmp(substring, "_Atomic") == 0) {
                tokens = token_push_back(tokens, cee__Atomic, substring, token_amount);
            }
            else if(strcmp(substring, "_Generic") == 0) {
                tokens = token_push_back(tokens, cee__Generic, substring, token_amount);
            }
            else if(strcmp(substring, "_Noreturn") == 0) {
                tokens = token_push_back(tokens, cee__Noreturn, substring, token_amount);
            }
            else if(strcmp(substring, "_Static_assert") == 0 || strcmp(substring, "static_assert") == 0) {
                tokens = token_push_back(tokens, cee_static_assert, substring, token_amount);
            }
            else if(strcmp(substring, "_Thread_local") == 0 || strcmp(substring, "thread_local") == 0) {
                tokens = token_push_back(tokens, cee_thread_local, substring, token_amount);
            }
            else if(strcmp(substring, "constexpr") == 0) {
                tokens = token_push_back(tokens, cee_constexpr, substring, token_amount);
            }
            else if(strcmp(substring, "false") == 0) {
                tokens = token_push_back(tokens, cee_false, substring, token_amount);
            }
            else if(strcmp(substring, "nullptr") == 0) {
                tokens = token_push_back(tokens, cee_nullptr, substring, token_amount);
            }
            else if(strcmp(substring, "true") == 0) {
                tokens = token_push_back(tokens, cee_true, substring, token_amount);
            }
            else if(strcmp(substring, "typeof") == 0) {
                tokens = token_push_back(tokens, cee_typeof, substring, token_amount);
            }
            else if(strcmp(substring, "typeof_unequal") == 0) {
                tokens = token_push_back(tokens, cee_typeof_unqual, substring, token_amount);
            }
            else if(strcmp(substring, "_BitInt") == 0) {
                tokens = token_push_back(tokens, cee__BitInt, substring, token_amount);
            }
            else if(strcmp(substring, "_Decimal32") == 0) {
                tokens = token_push_back(tokens, cee__Decimal32, substring, token_amount);
            }
            else if(strcmp(substring, "_Decimal64") == 0) {
                tokens = token_push_back(tokens, cee__Decimal64, substring, token_amount);
            }
            else if(strcmp(substring, "_Decimal128") == 0) {
                tokens = token_push_back(tokens, cee__Decimal128, substring, token_amount);
            }
            /* syntax error */
            else {
                printf("skill issue: haram syntax detected: %s\n", substring);
                exit(1);
            }
        }
        /* ignores whitespace */
        else if(isspace(str[i])) {
            continue;
        }
        /* numbers */
        else if(isdigit(str[i])) {
            buffer = char_push_back(buffer, str[i], i + 1);
            int pos = i;
            int num_length = 1;
            i++;
            while(isdigit(str[i])) {
                buffer = char_push_back(buffer, str[i], i + 1);
                num_length++;
                i++;
            }
            i--;
            token_amount++;
            char *substring = (char *)malloc(num_length + 1);
            memcpy(substring, buffer + pos, num_length + 1);
            substring[num_length] = '\0';
            tokens = token_push_back(tokens, cee_int_literal, substring, token_amount);
        }
        /* single line comments */
        else if(str[i] == '/' && str[i + 1] == '/') {
            while(str[i] != '\n') {
                i++;
            }
            i--;
            continue;
        }
        /* semicolons */
        else if(str[i] == ';') {
            buffer = char_push_back(buffer, str[i], i + 1);
            token_amount++;
            tokens = token_push_back(tokens, cee_semicolon, ";", token_amount);
        }
        /* syntax error */
        else {
            printf("skill issue: haram syntax detected\n");
            exit(1);
        }
    }
    free(buffer);

    return tokens;
}

char *char_push_back(char *str, char c, size_t size) {
    str = (char *)realloc(str, size);
    str[size - 1] = c;
    return str;
}

token_t *token_push_back(token_t *tok, token_type type, char *value, size_t size) {
    tok = (token_t *)realloc(tok, size * sizeof(token_t));
    tok[size - 1].type = type;
    tok[size - 1].value = strdup(value);
    return tok;
}
