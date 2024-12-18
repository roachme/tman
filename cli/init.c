#include <unistd.h>

#include "init.h"
#include "../src/tman.h"

int tman_init(int argc, char **argv)
{
    /* Generic check */
    if (access(tmanfs.finit, F_OK) == 0)
        return 0;

    if (MKDIR(tmanfs.base))
        return elog(1, "could not create directory %s", tmanfs.base);
    else if (MKDIR(tmanfs.base))
        return elog(1, "could not create directory %s", tmanfs.base);
    else if (MKDIR(tmanfs.pgn))
        return elog(1, "could not create directory %s", tmanfs.pgn);
    else if (MKDIR(tmanfs.pgnins))
        return elog(1, "could not create directory %s", tmanfs.pgnins);
    else if (MKDIR(tmanfs.db))
        return elog(1, "could not create directory %s", tmanfs.db);
    else if (TOUCH(tmanfs.fstate))
        return elog(1, "could not create file %s", tmanfs.fstate);
    else if (TOUCH(tmanfs.finit))
        return elog(1, "could not create init file %s", tmanfs.finit);
    return 0;
}
