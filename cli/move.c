#include <string.h>
#include "move.h"
#include "cli.h"

int tman_cli_move(int argc, char **argv, tman_ctx_t * ctx)
{
    char c;
    char *srcprj, *srcid, *dstprj, *dstid;

    srcprj = srcid = dstprj = dstid = NULL;
    while ((c = getopt(argc, argv, ":d:s:")) != -1) {
        switch (c) {
        case 'd':
            dstprj = optarg;
            break;
        case 's':
            srcprj = optarg;
            break;
        }
    }

    printf("argc: %d\n", argc);
    printf("optind: %d\n", optind);

    if ((srcprj == NULL && dstprj == NULL) || strcmp(srcprj, dstprj) == 0) {
        elog(1, "rename a task ID");
        if (argc > optind + 2) {
            elog(1, "err: can't rename more than 2 task at a time");
            return 1;
        }
    } else
        elog(1, "move a task ID");

    /*
       if (optind == 1 && (srcprj == NULL || dstprj == NULL)) {
       printf("can't move task: neither source nor destination prj specified\n");
       return 1;
       }
     */

    /*
     * Move:
     1. move a task to another prj.
     2. move multiple tasks to another prj.

     * Rename:
     2. rename task from current prj.
     3. rename task from another prj.
     */

    /*
     * tman move test1 test2 -d work (rename: if -s and -d are the same)
     * tman move test1 test2 -d work (move: if -s and -d are not the same)

     Special cases:
     * tman move test1 test2 test3 -d work (rename: if -s and -d are the same)
     *                                     Q: 3 and move task and -s & -d are not the same. Is it an error?
     *                                     A: not an error. Move task to destination prj.
     */

    int res =
        tman_id_move(NULL, srcprj, dstprj, argv[optind], argv[optind + 1]);
    if (res != TMAN_OK) {
        elog(1, "err[%d]: %s - %s", res, tman_strerror(), argv[optind]);
    }

    return 1;
}
