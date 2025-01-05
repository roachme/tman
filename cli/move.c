#include "move.h"
#include "cli.h"
#include <bits/getopt_core.h>

int tman_cli_move(int argc, char **argv, tman_ctx_t *ctx)
{
    char c;
    char *srcenv, *srcid, *dstenv, *dstid;

    srcenv = srcid = dstenv = dstid = NULL;
    while ((c = getopt(argc, argv, ":d:s:")) != -1) {
        switch (c) {
            case 'd':
                dstenv = optarg; break ;
            case 's':
                srcenv = optarg; break ;
        }
    }

    printf("argc: %d\n", argc);
    printf("optind: %d\n", optind);

    if (optind + 2 == argc) {
        printf("rename a task ID\n");
    } else {
        printf("move a task ID. (make sure srcenv and dstenv are diff\n");
    }

    if (optind == 1 && (srcenv == NULL || dstenv == NULL)) {
        printf("can't move task: neither source nor destination env specified\n");
        return 1;
    }




    /*
    dstenv = argv[--argc];
    for (int i = optind; i < argc; ++i) {
        char *id = argv[i];
        tman_id_move(ctx, id, dstenv, srcenv);
    }
    */

    return 1;
}
