#include "init.h"
#include "cli.h"

int tman_cli_init(int argc, char **argv, struct tman_context *ctx)
{
    int status;
    char *errfmt;

    errfmt = "could not make core filesystem: %s";
    if ((status = tman_mkfs()) != TMAN_OK)
        return elog(status, errfmt, tman_strerror());
    return TMAN_OK;
}
