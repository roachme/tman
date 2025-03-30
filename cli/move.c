#include <string.h>
#include "move.h"
#include "cli.h"

int tman_cli_move(int argc, char **argv, struct tman_context *ctx)
{
    char c;
    int i, status;
    struct tman_arg src = {
        .id = NULL,
        .brd = NULL,
        .prj = NULL,
    };
    struct tman_arg dst = {
        .id = NULL,
        .brd = NULL,
        .prj = NULL,
    };

    while ((c = getopt(argc, argv, ":d:s:")) != -1) {
        switch (c) {
        case 'd':
            dst.prj = optarg;
            break;
        case 's':
            src.prj = optarg;
            break;
        }
    }

    i = optind;
    tman_get_args(&src);
    tman_get_args(&dst);

    // some checks
    if ((argc - i) == 1) {
        elog(1, "destination task is missing");
        return 1;
    }

    if ((argc - i) == 2) {
        src.id = argv[i];
        dst.id = argv[i + 1];
        if ((status = tman_id_move(ctx, &src, &dst))) {
            return elog(1, "could not (re)move '%s': %s", src.id,
                        tman_strerror());
        }
    } else {
        do {
            src.id = dst.id = argv[i];
            if ((status = tman_id_move(ctx, &src, &dst))) {
                elog(1, "could not move '%s': %s", src.id, tman_strerror());
            }
        } while (++i < argc);
    }
    return 0;
}
