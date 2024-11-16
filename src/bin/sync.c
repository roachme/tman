#include "../core.h"
#include "../common.h"

int tman_sync(int argc, char **argv)
{
    int status;
    return (status = core_id_sync()) == TMAN_OK ? core_currdir() : status;
}
