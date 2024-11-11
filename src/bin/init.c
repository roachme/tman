#include <stdlib.h>
#include <unistd.h>

#include "init.h"
#include "../core/osdep.h"
#include "../core/common.h"

int tman_init(int argc, char **argv)
{
    char *homedir = getenv("HOME");
    // TODO: should get it from config file
    sprintf(tmanfs.base,  "%s/%s", homedir, "trash/tman");
    sprintf(tmanfs.db,    "%s/%s", tmanfs.base, ".tman");
    sprintf(tmanfs.finit, "%s/%s", tmanfs.db,   "inited");
    sprintf(tmanfs.fstate,"%s/%s", tmanfs.db,   "state");
    sprintf(tmanfs.task,  "%s/%s", tmanfs.base, "tasks");
    sprintf(tmanfs.pgn,   "%s/%s", tmanfs.base, "pgns");

    /* Generic check */
    if (access(tmanfs.finit, F_OK) == 0)
        return 0;

    if (MKDIR(tmanfs.base))
        return elog(1, "could not create directory %s", tmanfs.base);
    else if (MKDIR(tmanfs.task))
        return elog(1, "could not create directory %s", tmanfs.task);
    else if (MKDIR(tmanfs.pgn))
        return elog(1, "could not create directory %s", tmanfs.pgn);
    else if (MKDIR(tmanfs.db))
        return elog(1, "could not create directory %s", tmanfs.db);
    else if (TOUCH(tmanfs.fstate))
        return elog(1, "could not create file %s", tmanfs.fstate);
    else if (TOUCH(tmanfs.finit))
        return elog(1, "could not create init file %s", tmanfs.finit);
    return 0;
}
