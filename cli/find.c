#include "cli.h"
#include "find.h"

#include <bits/getopt_core.h>
#include <string.h>
#include <stdlib.h>


static int recursive_tree_print(struct tree *p)
{
    if (p != NULL) {
        recursive_tree_print(p->left);
        printf("%c %-10s [%s] %s\n", p->mark, p->id, p->pgnout, p->desc);
        recursive_tree_print(p->right);
    }
    return 0;
}

static int pretty_find(tman_ctx_t *ctx, char *prj, char *descpatt)
{
    if (tman_id_find_by_desc(ctx, prj, descpatt) != TMAN_OK) {
        elog(1, "could not list task IDs: %s", tman_strerror());
        return 1;
    }

    return recursive_tree_print(ctx->ids);
}

// TODO: Find a good error message in case option fails.  */
int tman_cli_find(int argc, char **argv, tman_ctx_t *ctx)
{
    char c, *prj, *pattern;
    int showhelp;

    showhelp = FALSE;
    prj = pattern = NULL;
    /* TODO: add case-sensetive option `-i'.  */
    while ((c = getopt(argc, argv, ":hp:")) != -1) {
        switch (c) {
            case 'h':
                showhelp = TRUE; break;
            case 'p':
                prj = optarg; break;
            case ':':
                return elog(1, "option `-%c' requires an argument", optopt);
            default:
                return elog(1, "invalid option `%c'", optopt);
        }
    }

    if (showhelp == 1)
        return help_usage("find");

    if ((pattern = argv[optind++]) == NULL) {
        elog(1, "PATTERN is missing");
        return 1;
    }
    return pretty_find(ctx, prj, pattern);
}
