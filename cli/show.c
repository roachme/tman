#include <string.h>

#include "show.h"
#include "cli.h"

static int pretty_show(struct tman_context *ctx, char *prj, char *id, char *key)
{
    int i, status;
    struct unit *unitbin, *unitpgn;
    struct tman_id_show_opt opt = { };

    if ((status = tman_id_show(ctx, prj, id, &opt)) != TMAN_OK)
        return elog(status, "cannot show units '%s': %s", id, tman_strerror());

    printf("%-7s : %s\n", "id", ctx->units.id);

    for (unitbin = ctx->units.bin, i = 0; i < NKEYS; ++i)
        printf("%-7s : %s\n", unitbin[i].key, unitbin[i].val);

    for (unitpgn = ctx->units.pgn; unitpgn != NULL; unitpgn = unitpgn->next)
        printf("%-7s : %s\n", unitpgn->key, unitpgn->val);

    return TMAN_OK;
}

int tman_cli_show(int argc, char **argv, struct tman_context *ctx)
{
    char c;
    int i, status;
    struct tman_cli_show_opt opt = {.prj = NULL,.help = 0,.force = 0, };

    while ((c = getopt(argc, argv, ":p:hk")) != -1) {
        switch (c) {
        case 'p':
            opt.prj = optarg;
            break;
        case 'k':
            opt.key = optarg;
            break;
        case ':':
            return elog(1, "option `-%c' requires an argument", optopt);
        default:
            return elog(1, "invalid option `%c'", optopt);
        }
    }

    i = optind;
    do {
        status = pretty_show(ctx, opt.prj, argv[i], opt.key);
    } while (++i < argc);
    return status;
}
