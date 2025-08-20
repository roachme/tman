#include "tec.h"
#include "aux/config.h"

int tec_cli_init(int argc, char **argv, tec_ctx_t * ctx)
{
    int status;

    if ((status = tec_make_db(teccfg.base.task)) != LIBTEC_OK) {
        const char *errfmt = "could not make core filesystem: %s";
        return elog(status, errfmt, tec_strerror(status));
    }
    return LIBTEC_OK;
}
