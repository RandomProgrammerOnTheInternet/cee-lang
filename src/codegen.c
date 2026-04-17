#include "codegen.h"
#include "codegen_x86.h"
#include "codegen_arm64.h"

FILE *generate_asm(LIST(node_base_t) node, enum backend backend) {
    switch(backend){
        case backend_x86:
            return generate_asm_x86(node);
        case backend_arm64:
            return generate_asm_arm64(node);
        default:
            return NULL;
    }    

    return NULL;
}
