#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>
#include <string.h>
#include "ds.h"
#include "lexer.h"
#include "parser.h"


enum backend : u8 {
    backend_x86,
    backend_arm64,
};

FILE *generate_asm(LIST(node_base_t) node, enum backend backend);

#endif // CODEGEN_H
