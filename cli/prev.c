#include "prev.h"
#include "../src/tman.h"

int tman_cli_prev(int argc, char **argv)
{
    int status;
    char *errfmt;

    errfmt =  "cannot switch: %s";
    if ((status = tman_id_prev()) != TMAN_OK)
        return elog(1, errfmt, tman_get_errmsg());
    return tman_pwd();
}
