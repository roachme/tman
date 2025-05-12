#include "cli.h"

int tman_cli_init(int argc, char **argv, struct tman_context *ctx)
{
    int status;

    if ((status = tman_mkfs()) != TMAN_OK) {
        const char *errfmt = "could not make core filesystem: %s";
        return elog(status, errfmt, tman_strerror());
    }
    return TMAN_OK;
}
