#include "../core/core.h"
#include "../core/common.h"

int tman_sync(int argc, char **argv)
{
    if (core_id_sync())
        return elog("no current task is set");
    return core_currdir();
}
