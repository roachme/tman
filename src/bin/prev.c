#include "prev.h"
#include "../common.h"
#include "../core.h"

int tman_prev(int argc, char **argv)
{
    if (core_id_prev())
        return elog(1, "no previous task is set");
    return core_currdir();
}
