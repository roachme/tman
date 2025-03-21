#include "cli.h"

int tman_cli_ver(int argc, char **argv, struct tman_context *ctx)
{
    printf("%s: %s\n", PROGRAM, VERSION);
    return TMAN_OK;
}
