#include "util.h"
#include "lexer.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int token_amount = 0;

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
                        safe_free(modified);
                    }
                }
            }
            if(!found) {
                tokens = token_push_back(tokens, cee_identifier, substring, token_amount);
            }
            safe_free(substring);

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
            safe_free(substring); 
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
    safe_free(buffer);

    return tokens;
}

token_t *token_push_back(token_t *tok, token_type type, char *value, size_t size) {
    tok = (token_t *)realloc(tok, size * sizeof(token_t));
    tok[size - 1].type = type;
    tok[size - 1].value = strdup(value);
    return tok;
}
