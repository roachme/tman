#include "prev.h"
#include "../src/tman.h"

int tman_cli_prev(int argc, char **argv)
{
    if (tman_id_prev())
        return elog(1, "no previous task is set");
    return tman_pwd();
}
