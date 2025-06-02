#include <string.h>

#include "cli.h"
#include "config.h"

static const char *errfmt = "cannot show units '%s': %s";

static int show_key(struct tman_context *ctx, char *key)
{
    struct unit *unitbin, *unitpgn;

    if (strcmp(key, "id") == 0) {
        printf("%s\n", ctx->id);
        return LIBTMAN_OK;
    }

    for (unitbin = ctx->unitbin; unitbin; unitbin = unitbin->next)
        if (strcmp(key, unitbin->key) == 0) {
            printf("%s\n", unitbin->val);
            return LIBTMAN_OK;
        }

    /* Look up addition fields.  */
    if (strcmp(key, "parent") == 0) {
        printf("%s\n", ctx->linkparent);
        return 0;
    } else if (strcmp(key, "child") == 0) {
        printf("%s\n", ctx->linkchild);
        return 0;
    }

    for (unitpgn = ctx->unitpgn; unitpgn; unitpgn = unitpgn->next)
        if (strcmp(key, unitpgn->key) == 0) {
            printf("%s\n", unitpgn->val);
            return LIBTMAN_OK;
        }

    return 1;
}

static int pretty_show(struct tman_context *ctx, int showaddition)
{
    struct unit *unitbin, *unitpgn;

    printf("%-7s : %s\n", "id", ctx->id);

    for (unitbin = ctx->unitbin; unitbin; unitbin = unitbin->next)
        printf("%-7s : %s\n", unitbin->key, unitbin->val);

    /* Show links if needed.  */
    if (showaddition) {
        printf("%-7s : %s\n", "parent", ctx->linkparent);
        printf("%-7s : %s\n", "child", ctx->linkchild);
    }

    for (unitpgn = ctx->unitpgn; unitpgn; unitpgn = unitpgn->next)
        printf("%-7s : %s\n", unitpgn->key, unitpgn->val);

    return LIBTMAN_OK;
}

int tman_cli_show(int argc, char **argv, struct tman_context *ctx)
{
    char c;
    char *key;
    int i, quiet, showaddition, showhelp, status;
    struct tman_arg args;

    quiet = showaddition = showhelp = FALSE;
    key = args.prj = args.id = args.brd = NULL;
    while ((c = getopt(argc, argv, ":ap:hk:q")) != -1) {
        switch (c) {
        case 'a':
            showaddition = TRUE;
            break;
        case 'h':
            showhelp = TRUE;
            break;
        case 'k':
            key = optarg;
            break;
        case 'p':
            args.prj = optarg;
            break;
        case 'q':
            quiet = TRUE;
            break;
        case ':':
            return elog(1, "option `-%c' requires an argument", optopt);
        default:
            return elog(1, "invalid option `-%c'", optopt);
        }
    }

    if (showhelp == TRUE)
        return help_usage("show");

    i = optind;
    do {
        args.id = argv[i];

        if ((status = tman_task_show(ctx, &args, NULL)) != LIBTMAN_OK) {
            args.id = args.id ? args.id : "NOCURR";
            if (quiet == FALSE)
                elog(status, errfmt, args.id, tman_strerror());
            continue;
        } else if (tman_config->usehooks == TRUE
                   && tman_hook_show(ctx, tman_config->hooks, &args,
                                     "show") == NULL) {
            if (quiet == FALSE)
                elog(status, errfmt, args.id, tman_strerror());
            continue;
        }

        if (key != NULL) {
            if (show_key(ctx, key) != LIBTMAN_OK && quiet == FALSE) {
                if (quiet == FALSE)
                    elog(1, errfmt, args.id, "key not found");
            }
        } else {
            pretty_show(ctx, showaddition);
        }

        ctx->unitbin = tman_unit_free(ctx, &args, NULL);
        ctx->unitpgn = tman_hook_show_free(ctx, &args);
    } while (++i < argc);

    return status;
}
