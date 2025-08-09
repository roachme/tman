#include "cli.h"
#include "aux/config.h"

int tman_cli_init(int argc, char **argv, tman_ctx_t * ctx)
{
    int status;

    if ((status = tman_make_db(tmancfg.base.task)) != LIBTMAN_OK) {
        const char *errfmt = "could not make core filesystem: %s";
        return elog(status, errfmt, tman_strerror(status));
    }
    return LIBTMAN_OK;
}
