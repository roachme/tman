#include "cli.h"

int tman_cli_ver(int argc, char **argv, tman_ctx_t *ctx)
{
    printf("%s: %s\n", PROGRAM, VERSION);
    return TMAN_OK;
}
