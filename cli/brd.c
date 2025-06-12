#include "cli.h"

int tman_cli_brd(int argc, char **argv, struct tman_context *ctx)
{
    tman_pwd_unset();
    return elog(1, "feature 'board' is under development");
}
