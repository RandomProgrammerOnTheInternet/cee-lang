#include "codegen.h"
#include "codegen_x86.h"

FILE *generate_asm(LIST(node_base_t) node, enum backend backend) {
    LOG(PRN_YLW, "called generate_asm");
    switch(backend){
        case backend_x86:
            return generate_asm_x86(node);
        case backend_arm64:
        default:
            LOG(PRN_YLW, "ERROR");
            return NULL;
    }
    LOG(PRN_YLW, "ERROR");

    return NULL;
}
