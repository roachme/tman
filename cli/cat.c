#include <string.h>

#include "cat.h"
#include "cli.h"

static int pretty_cat(tman_ctx_t *ctx, char *env, char *id, char *key)
{
    int i, status;
    struct unit *unitbin, *unitpgn;
    struct tman_id_cat_opt opt = { };

    if ((status = tman_id_cat(ctx, env, id, &opt)) != TMAN_OK)
        return elog(status, "cannot cat units '%s': %s", id, tman_strerror());

    printf("%-7s : %s\n", "id", ctx->units.id);

    for (unitbin = ctx->units.bin, i = 0; i < NKEYS; ++i)
        printf("%-7s : %s\n", unitbin[i].key, unitbin[i].val);

    for (unitpgn = ctx->units.pgn; unitpgn != NULL; unitpgn = unitpgn->next)
        printf("%-7s : %s\n", unitpgn->key, unitpgn->val);

    return TMAN_OK;
}

int tman_cli_cat(int argc, char **argv, tman_ctx_t *ctx)
{
    char c;
    int i, status;
    struct tman_cli_cat_opt opt = { .env = NULL, .help = 0, .force = 0, };

    while ((c = getopt(argc, argv, ":e:hk")) != -1) {
        switch (c) {
            case 'e': opt.env = optarg; break;
            case 'k': opt.key = optarg; break;
            case ':':
                return elog(1, "option `-%c' requires an argument", optopt);
            default:
                return elog(1, "invalid option `%c'", optopt);
        }
    }

    i = optind;
    do {
        status = pretty_cat(ctx, opt.env, argv[i], opt.key);
    } while (++i < argc);
    return status;
}
