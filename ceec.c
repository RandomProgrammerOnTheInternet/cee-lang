#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

typedef enum {
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

const char *keyword_table[] = {
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

typedef struct {
    token_type type;
    char *value;
} token_t;

/* nodes */
typedef enum {
    tag_int_lit = 0,
    tag_expr,
    tag_return
} node_tag;
typedef struct {
    token_t token;
} int_lit_t;
typedef struct {
    int_lit_t int_lit;
} expr_t;

typedef struct {
    expr_t expr;
} return_t;

typedef struct {
    node_tag tag;
    union {
        int_lit_t int_lit;
        expr_t expr;
        return_t ret;
    };
} node_t;

char *read_whole_file(FILE *);
token_t *tokenize(char *);
node_t *parse(token_t *);
FILE *generate(node_t *);
char *char_push_back(char *, char, size_t);
token_t *token_push_back(token_t *, token_type, char *, size_t);
node_t *node_push_back(node_t *, size_t);

int token_amount = 0;
int node_amount = 0;

int main(int argc, char **argv) {
    if(argc == 1) {
        printf("skill issue: you gotta put a file\n");
        exit(1);
    }
    FILE *file = NULL;
    if(!(file = fopen(argv[1], "r"))) {
        printf("skill issue: invalid file\n");
        exit(1);
    }
    char *str = read_whole_file(file);
    token_t *tokens = tokenize(str);
    node_t *nodes = parse(tokens);
    FILE *output = generate(nodes);
    fclose(output);
    system("nasm -felf64 out.asm");
    system("ld -o a.out out.o");
    for(int i = 0; i < token_amount; i++) {
        printf("Token %d: ", i);
        printf("%s\n", tokens[i].value);
    }
    printf("-------------------------------\nAmount of nodes: %d\n", node_amount);
    printf("-------------------------------\n");

    for(int i = 0; i < token_amount; i++) {
        printf("%d\n", tokens[i].type);
    }
    for(int i = 0; i < token_amount; i++) {
        free(tokens[i].value);
    }
    free(tokens);
    free(str);
    free(nodes);
    fclose(file);

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
            char *substring = (char *)malloc(length + 16);
            memset(substring, 0, length + 16);
            memcpy(substring, buffer + pos, length);
            i--;
            int found = 0;
            for(int j = 0; j < cee_keyword_count - 1; j++) {
                if(keyword_table[j]) {
                    if(strcmp(substring, keyword_table[j]) == 0) {
                        found = 1;
                        tokens = token_push_back(tokens, j, substring, token_amount);
                    }
                    else if(keyword_table[j][0] == '_') {
                        char *modified = strdup(keyword_table[j]);
                        modified++;
                        modified[0] = tolower(modified[0]);
                        if(strcmp(substring, modified) == 0) {
                            found = 1;
                            tokens = token_push_back(tokens, j, substring, token_amount);
                        }
                        modified--;
                        free(modified);
                    }
                }
            }
            if(!found) {
                tokens = token_push_back(tokens, cee_identifier, substring, token_amount);
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
            memcpy(substring, buffer + pos, num_length);
            substring[num_length] = '\0';
            tokens = token_push_back(tokens, cee_int_literal, substring, token_amount);
            free(substring); 
        }
        /* single line comments */
        else if(str[i] == '/' && str[i + 1] == '/') {
            while(str[i] != '\n') {
                i++;
            }
            i--;
            continue;
        }
        else {
            switch(str[i]) {
                /* semicolons */
                case ';':
                    buffer = char_push_back(buffer, str[i], i + 1);
                    token_amount++;
                    tokens = token_push_back(tokens, cee_semicolon, ";", token_amount);
                    break;
                case '{':
                    buffer = char_push_back(buffer, str[i], i + 1);
                    token_amount++;
                    tokens = token_push_back(tokens, cee_left_curly_brace, "{", token_amount);
                    break;
                case '}':
                    buffer = char_push_back(buffer, str[i], i + 1);
                    token_amount++;
                    tokens = token_push_back(tokens, cee_right_curly_brace, "}", token_amount);
                case '<':
                    if(str[i + 1] == '<') {
                        if(str[i + 2] == '=') {
                            buffer = char_push_back(buffer, str[i], i + 3);
                            token_amount++;
                            tokens = token_push_back(tokens, cee_OP_assignment_bitshift_left, "<<=", token_amount);
                            i+=2;
                        }
                        else {
                            buffer = char_push_back(buffer, str[i], i + 2);
                            token_amount++;
                            tokens = token_push_back(tokens, cee_OP_bitshift_left, "<<", token_amount);
                            i++;
                        }
                    }
                    else if(str[i + 1] == '=') {
                        buffer = char_push_back(buffer, str[i], i + 2);
                        token_amount++;
                        tokens = token_push_back(tokens, cee_OP_less_than_equal_to, "<=", token_amount);
                        i++;
                    }
                    else {
                        buffer = char_push_back(buffer, str[i], i + 1);
                        token_amount++;
                        tokens = token_push_back(tokens, cee_OP_less_than, "<", token_amount);
                    }
                    break;
                case '>':
                    if(str[i + 1] == '>') {
                        if(str[i + 2] == '=') {
                            buffer = char_push_back(buffer, str[i], i + 3);
                            token_amount++;
                            tokens = token_push_back(tokens, cee_OP_assignment_bitshift_right, ">>=", token_amount);
                            i+=2;
                        }
                        else {
                            buffer = char_push_back(buffer, str[i], i + 2);
                            token_amount++;
                            tokens = token_push_back(tokens, cee_OP_bitshift_left, ">>", token_amount);
                            i++;
                        }
                    }
                    else if(str[i + 1] == '=') {
                        buffer = char_push_back(buffer, str[i], i + 2);
                        token_amount++;
                        tokens = token_push_back(tokens, cee_OP_greater_than_equal_to, ">=", token_amount);
                        i++;
                    }
                    else {
                        buffer = char_push_back(buffer, str[i], i + 1);
                        token_amount++;
                        tokens = token_push_back(tokens, cee_OP_greater_than, ">", token_amount);
                    }
                    break;
                case '+':
                    if(str[i + 1] == '=') {
                        buffer = char_push_back(buffer, str[i], i + 2);
                        token_amount++;
                        tokens = token_push_back(tokens, cee_OP_assignment_add, "+=", token_amount);
                        i++;
                    }
                    else if(str[i + 1] == '+') {
                        buffer = char_push_back(buffer, str[i], i + 2);
                        token_amount++;
                        tokens = token_push_back(tokens, cee_OP_increment, "++", token_amount);
                        i++;
                    }
                    else {
                        buffer = char_push_back(buffer, str[i], i + 1);
                        token_amount++;
                        tokens = token_push_back(tokens, cee_OP_plus, "+", token_amount);
                    }
                    break;
                case '-':
                    if(str[i + 1] == '=') {
                        buffer = char_push_back(buffer, str[i], i + 2);
                        token_amount++;
                        tokens = token_push_back(tokens, cee_OP_assignment_subtract, "-=", token_amount);
                        i++;
                    }
                    else if(str[i + 1] == '-') {
                        buffer = char_push_back(buffer, str[i], i + 2);
                        token_amount++;
                        tokens = token_push_back(tokens, cee_OP_decrement, "--", token_amount);
                        i++;
                    }
                    else if(str[i + 1] == '>') {
                        buffer = char_push_back(buffer, str[i], i + 2);
                        token_amount++;
                        tokens = token_push_back(tokens, cee_OP_pointer_member_select, "->", token_amount);
                        i++;
                    }
                    else {
                        buffer = char_push_back(buffer, str[i], i + 1);
                        token_amount++;
                        tokens = token_push_back(tokens, cee_OP_minus, "-", token_amount);
                    }
                    break;
                case '*':
                    if(str[i + 1] == '=') {
                        buffer = char_push_back(buffer, str[i], i + 2);
                        token_amount++;
                        tokens = token_push_back(tokens, cee_OP_assignment_multiply, "*=", token_amount);
                        i++;
                    }
                    else {
                        buffer = char_push_back(buffer, str[i], i + 1);
                        token_amount++;
                        tokens = token_push_back(tokens, cee_OP_asterisk, "*", token_amount);
                    }
                    break;
                case '/':
                    if(str[i + 1] == '=') {
                        buffer = char_push_back(buffer, str[i], i + 2);
                        token_amount++;
                        tokens = token_push_back(tokens, cee_OP_assignment_divide, "/=", token_amount);
                        i++;
                    }
                    else {
                        buffer = char_push_back(buffer, str[i], i + 1);
                        token_amount++;
                        tokens = token_push_back(tokens, cee_OP_slash, "/", token_amount);
                    }
                    break;
                case '%':
                    if(str[i + 1] == '=') {
                        buffer = char_push_back(buffer, str[i], i + 2);
                        token_amount++;
                        tokens = token_push_back(tokens, cee_OP_assignment_modulus, "%=", token_amount);
                        i++;
                    }
                    else {
                        buffer = char_push_back(buffer, str[i], i + 1);
                        token_amount++;
                        tokens = token_push_back(tokens, cee_OP_percent, "%", token_amount);
                    }
                    break;
                case '=':
                    if(str[i + 1] == '=') {
                        buffer = char_push_back(buffer, str[i], i + 2);
                        token_amount++;
                        tokens = token_push_back(tokens, cee_OP_equals, "==", token_amount);
                        i++;
                    }
                    else {
                        buffer = char_push_back(buffer, str[i], i + 1);
                        token_amount++;
                        tokens = token_push_back(tokens, cee_OP_assignment_equals, "=", token_amount);
                    }
                    break;
                case '!':
                    if(str[i + 1] == '=') {
                        buffer = char_push_back(buffer, str[i], i + 2);
                        token_amount++;
                        tokens = token_push_back(tokens, cee_OP_NOT_equals, "!=", token_amount);
                        i++;
                    }
                    else {
                        buffer = char_push_back(buffer, str[i], i + 1);
                        token_amount++;
                        tokens = token_push_back(tokens, cee_OP_boolean_NOT, "!", token_amount);
                    }
                    break;
                case '&':
                    if(str[i + 1] == '&') {
                        buffer = char_push_back(buffer, str[i], i + 2);
                        token_amount++;
                        tokens = token_push_back(tokens, cee_OP_boolean_AND, "&&", token_amount);
                        i++;
                    }
                    else if(str[i + 1] == '='){
                        buffer = char_push_back(buffer, str[i], i + 2);
                        token_amount++;
                        tokens = token_push_back(tokens, cee_OP_assignment_bitwise_AND, "&=", token_amount);
                        i++;
                    }
                    else {
                        buffer = char_push_back(buffer, str[i], i + 1);
                        token_amount++;
                        tokens = token_push_back(tokens, cee_OP_ampersand, "&", token_amount);
                    }
                    break;
                case '|':
                    if(str[i + 1] == '|') {
                        buffer = char_push_back(buffer, str[i], i + 2);
                        token_amount++;
                        tokens = token_push_back(tokens, cee_OP_boolean_OR, "||", token_amount);
                        i++;
                    }
                    else if(str[i + 1] == '='){
                        buffer = char_push_back(buffer, str[i], i + 2);
                        token_amount++;
                        tokens = token_push_back(tokens, cee_OP_assignment_bitwise_OR, "|=", token_amount);
                        i++;
                    }
                    else {
                        buffer = char_push_back(buffer, str[i], i + 1);
                        token_amount++;
                        tokens = token_push_back(tokens, cee_OP_bitwise_OR, "|", token_amount);
                    }
                    break;
                case '^':
                    if(str[i + 1] == '='){
                        buffer = char_push_back(buffer, str[i], i + 2);
                        token_amount++;
                        tokens = token_push_back(tokens, cee_OP_assignment_bitwise_XOR, "^=", token_amount);
                        i++;
                    }
                    else {
                        buffer = char_push_back(buffer, str[i], i + 1);
                        token_amount++;
                        tokens = token_push_back(tokens, cee_OP_bitwise_XOR, "^", token_amount);
                    }
                    break;
                case '~':
                    buffer = char_push_back(buffer, str[i], i + 1);
                    token_amount++;
                    tokens = token_push_back(tokens, cee_OP_bitwise_NOT, "~", token_amount);
                    break;
                case '?':
                    buffer = char_push_back(buffer, str[i], i + 1);
                    token_amount++;
                    tokens = token_push_back(tokens, cee_OP_question_mark, "?", token_amount);
                    break;
                case ':':
                    buffer = char_push_back(buffer, str[i], i + 1);
                    token_amount++;
                    tokens = token_push_back(tokens, cee_OP_colon, "^", token_amount);
                    break;
                case '(':
                    buffer = char_push_back(buffer, str[i], i + 1);
                    token_amount++;
                    tokens = token_push_back(tokens, cee_OP_left_parentheses, "(", token_amount);
                    break;
                case ')':
                    buffer = char_push_back(buffer, str[i], i + 1);
                    token_amount++;
                    tokens = token_push_back(tokens, cee_OP_right_parentheses, ")", token_amount);
                    break;
                case '.':
                    buffer = char_push_back(buffer, str[i], i + 1);
                    token_amount++;
                    tokens = token_push_back(tokens, cee_OP_member_select, ".", token_amount);
                    break;
                case '[':
                    buffer = char_push_back(buffer, str[i], i + 1);
                    token_amount++;
                    tokens = token_push_back(tokens, cee_OP_left_bracket, "[", token_amount);
                    break;
                case ']':
                    buffer = char_push_back(buffer, str[i], i + 1);
                    token_amount++;
                    tokens = token_push_back(tokens, cee_OP_right_bracket, "]", token_amount);
                case ',':
                    buffer = char_push_back(buffer, str[i], i + 1);
                    token_amount++;
                    tokens = token_push_back(tokens, cee_OP_comma, ",", token_amount);
                    break;
                default:
                    printf("skill issue: haram syntax detected\n");
                    exit(1);
            }
        }
    }
    free(buffer);

    return tokens;
}

node_t *parse(token_t *tokens) {
    node_t *nodes = (node_t *)malloc(0);
    for(int i = 0; i < token_amount; i++) {
        if(tokens[i].type == cee_return) {
            node_amount++;
            nodes = node_push_back(nodes, node_amount);
            i++;
            if(tokens[i].type == cee_int_literal) {
                node_amount++;
                nodes = node_push_back(nodes, node_amount);
                i++;
                node_amount++;
                nodes = node_push_back(nodes, node_amount);
                if(tokens[token_amount - 1].type != cee_semicolon || tokens[i].type != cee_semicolon) {
                    printf("skill issue: missing semicolon on line idk, go find it yourself\n");
                    exit(1);
                }
                nodes[i].int_lit.token = tokens[i - 1];
                nodes[i].tag = tag_int_lit;
                nodes[i - 1].expr.int_lit = nodes[i].int_lit;
                nodes[i - 1].tag = tag_expr;
                nodes[i - 2].ret.expr = nodes[i - 1].expr;
                nodes[i - 2].tag = tag_return;
            }
            else {
                printf("skill issue: you have to return an EXPRESSION, not what you just put\n");
                exit(1);
            }
        }
    }

    return nodes;
}

FILE *generate(node_t *nodes) {
    FILE *f = fopen("out.asm", "w+");
    fprintf(f, "global _start\n");
    fprintf(f, "_start:\n");
    for(int i = 0; i < node_amount; i++) {
        if(nodes[i].tag == tag_return) {
            fprintf(f, "    mov rax, 60\n");
            i++;
            fprintf(f, "    mov rdi, %s\n", nodes[i + 1].expr.int_lit.token.value);
            i++;
            fprintf(f, "    syscall\n");
        }
    }

    return f;
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

node_t *node_push_back(node_t *node, size_t size) {
    node = (node_t *)realloc(node, size * sizeof(node_t));
    return node;
}
