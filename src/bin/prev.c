#include "prev.h"
#include "../core/common.h"
#include "../core/core.h"

int tman_prev(int argc, char **argv)
{
    if (core_id_prev())
        return elog("no previous task is set");
    return core_currdir();
}
