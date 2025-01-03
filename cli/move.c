#include "move.h"
#include "cli.h"

int tman_cli_move(int argc, char **argv, tman_ctx_t *ctx)
{
    char c;
    char *srcenv = NULL;
    char *dstenv = NULL;

    while ((c = getopt(argc, argv, "s:")) != -1) {
        switch (c) {
            case 's':
                srcenv = optarg; break ;
        }
    }

    if (optind == argc)
        return elog(1, "task id required");
    if (optind + 1 == argc)
        return elog(1, "destination env required");

    dstenv = argv[--argc];
    for (int i = optind; i < argc; ++i) {
        char *id = argv[i];
        tman_id_move(ctx, id, dstenv, srcenv);
    }

    return 1;
}
