#include <string.h>

#include "cat.h"
#include "cli.h"

static int pretty_cat(tman_ctx_t *ctx, char *env, char *id, char *key)
{
    int i, status;
    struct unitbin *unitbin;
    struct unitpgn *unitpgn;
    struct tman_id_cat_opt opt = { };

    if ((status = tman_id_cat(ctx, env, id, &opt)) != TMAN_OK)
        return elog(status, "cannot cat units '%s': %s", id, tman_strerror());

    unitbin = ctx->units.bin;
    for (i = 0; i < NKEYS; ++i)
        printf("%-7s : %s\n", unitbin[i].key, unitbin[i].val);

    unitpgn = ctx->units.pgn;
    while (unitpgn != NULL) {
        printf("%-7s : %s\n", unitpgn->node.key, unitpgn->node.val);
        unitpgn = unitpgn->next;
    }
    return TMAN_OK;
}

int tman_cli_cat(int argc, char **argv, tman_ctx_t *ctx)
{
    char c;
    int i, status;
    struct tman_cli_cat_opt opt = { .env = NULL, .help = 0, .force = 0, };

    while ((c = getopt(argc, argv, ":e:fhk")) != -1) {
        switch (c) {
            case 'e': opt.env = optarg; break;
            case 'f': opt.force = 1; break;
            case 'k': opt.key = optarg; break;
            case ':':
                return elog(1, "option `-%c' requires an argument", optopt);
            default:
                return elog(1, "invalid option `%c'", optopt);
        }
    }

    i = optind;
    do {
        if ((status = pretty_cat(ctx, opt.env, argv[i], opt.key)) != TMAN_OK)
            return status;
        ++i;
    } while (i < argc);
    return status;
}
