#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdint.h>

#include "util.h"
#include "lexer.h"

void parse(token_t *);
//FILE *generate(node_t *);
void compile(int, char **);
//node_t *node_push_back(node_t *, size_t);

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
    //node_t *nodes = parse(tokens);
    //FILE *output = generate(nodes);
    //fclose(output);
    //system("nasm -felf64 out.asm");
    //system("ld -o a.out out.o");
    for(int i = 0; i < token_amount; i++) {
        printf("Token %d: ", i);
        printf("%s\n", tokens[i].value);
    }
    printf("-------------------------------\nAmount of nodes: NULL\n");
    printf("-------------------------------\n");

    for(int i = 0; i < token_amount; i++) {
        printf("%d\n", tokens[i].type);
    }
    for(int i = 0; i < token_amount; i++) {
        safe_free(tokens[i].value);
    }
    safe_free(tokens);
    safe_free(str);
    //safe_free(nodes);
    fclose(file);

    return 0;
}

/*
FILE *generate(node_t *nodes) {
    FILE *f = fopen("out.asm", "w+");
    fprintf(f, "global _start\n");
    fprintf(f, "_start:\n");
    for(int i = 0; i < node_amount; i++) {
        if(nodes[i].tag == node_return) {
            fprintf(f, "    mov rax, 60\n");
            i++;
            nodes[i].tag = node_expr;
            fprintf(f, "    mov rdi, %s\n", nodes[i + 1].int_lit.token.value);
            i++;
            nodes[i].tag = node_int_lit;
            fprintf(f, "    syscall\n");
        }
    }

    return f;
}
*/
/*
node_t *node_push_back(node_t *node, size_t size) {
    node = (node_t *)realloc(node, size * sizeof(node_t));
    return node;
}
*/
