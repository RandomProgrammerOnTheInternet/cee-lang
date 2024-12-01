#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum {
    cee_int_literal = 0,
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
    cee__Decimal128,

    cee_keyword_count
} token_type;

const char *keyword_table[] = {
    NULL, // cee_int_literal
    NULL, // cee_semicolon
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
            
            int found = 0;
            for(int i = 0; i < cee_keyword_count; i++) {
                if(keyword_table[i]) {
                    if(strcmp(substring, keyword_table[i]) == 0) {
                        found = 1;
                        tokens = token_push_back(tokens, i, substring, token_amount);
                    } else if(keyword_table[i][0] == '_') {
                        char *modified = strdup(keyword_table[i]);
                        modified++;
                        modified[0] = tolower(modified[0]);
                        if(strcmp(substring, modified) == 0) {
                            found = 1;
                            tokens = token_push_back(tokens, i, substring, token_amount);
                        }
                        modified--;
                        free(modified);
                    }
                }
            }
            if(!found) {
                printf("skill issue: haram syntax detected: %s\n",substring);
                exit(EXIT_FAILURE);
            }
            free(substring);
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
