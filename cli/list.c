#include <string.h>
#include <stdlib.h>

#include "list.h"
#include "cli.h"

static int tman_cli_list_usage(void)
{
    const char *cmd = "help";
    printf("Usage: %s %s [OPTION]... [ID]..\n", PROGRAM, cmd);
    printf("Try '%s help %s' for more info.\n", PROGRAM, cmd);
    return TMAN_OK;
}

static int compare(const void *aa, const void *bb)
{
    struct ilist *a = (struct ilist*)aa;
    struct ilist *b = (struct ilist*)bb;
    return (a->col.prio - b->col.prio);
}

static int recursive_tree_print(struct tree *p)
{
    if (p != NULL) {
        recursive_tree_print(p->left);
        printf("%c %-10s [%s] %s\n", p->mark, p->id, p->pgnout, p->desc);
        recursive_tree_print(p->right);
    }
    return 0;
}

static int pretty_list(tman_ctx_t *ctx, char *env, struct tman_id_list_opt *opt)
{
    if (tman_id_list(ctx, env, opt) != TMAN_OK) {
        elog(1, "could not list task IDs: %s", tman_strerror());
        return 1;
    }

    return recursive_tree_print(ctx->tree);
}

// TODO: Find a good error message in case option fails.  */
int tman_cli_list(int argc, char **argv, tman_ctx_t *ctx)
{
    char c;
    int showhelp, status;
    struct tman_id_list_opt opt = { };

    /*
        -A - list all (even done tasks)
        -a - almost all (everything but done tasks)
        -c - specify what column to list
        -s - default: list only current & previous (maybe?)
    */
    showhelp = FALSE;
    while ((c = getopt(argc, argv, ":Aac:hs")) != -1) {
        switch (c) {
            case ':':
                return elog(1, "option `-%c' requires an argument", optopt);
            default:
                return elog(1, "invalid option `%c'", optopt);
        }
    }

    // TODO: check that option don't conflict with each other.

    if (showhelp == 1)
        return tman_cli_list_usage();

    for (int i = optind; i < argc; ++i) {
        char *env = argv[i];
        status = pretty_list(ctx, env, &opt);
    }

    /* if no arguments passed then list current env */
    return optind < argc ? status : pretty_list(ctx, NULL, &opt);
}
