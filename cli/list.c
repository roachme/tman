#include <string.h>
#include <stdlib.h>

#include "list.h"
#include "cli.h"
#include "color.h"

static const char *errfmt = "cannot list project tasks '%s': %s";

struct list_filter {
    int allall;
    int almostall;
    int specialonly;
    char *column;
};

static struct list_filter list_filter = {
    .allall = FALSE,
    .almostall = TRUE,
    .specialonly = FALSE,
    .column = NULL,
};

static int recursive_tree_print(struct tree *p)
{
    if (p != NULL) {
        recursive_tree_print(p->left);

        /* Apply filters */
        if (list_filter.specialonly == TRUE
            && (p->mark == '*' || p->mark == '^'))
            printf("%c " BMAG "%-" xstr(IDSIZ) "s" CRESET "%s %s\n", p->mark,
                   p->id, p->pgnout, p->desc);
        else if (list_filter.almostall == TRUE && (p->mark != '-'))
            printf("%c " BMAG "%-" xstr(IDSIZ) "s" CRESET "%s %s\n", p->mark,
                   p->id, p->pgnout, p->desc);
        else if (list_filter.allall == TRUE)
            printf("%c " BMAG "%-" xstr(IDSIZ) "s" CRESET "%s %s\n", p->mark,
                   p->id, p->pgnout, p->desc);

        recursive_tree_print(p->right);
    }
    return 0;
}

// TODO: Find a good error message in case option fails.  */
int tman_cli_list(int argc, char **argv, struct tman_context *ctx)
{
    char c;
    struct tman_arg args;
    int i, showhelp, status;

    /*
       -A - list all (even done tasks)
       -a - almost all (everything but done tasks)
       -c - specify what column to list
       -s - default: list only current & previous (maybe?)
     */
    showhelp = FALSE;
    args.id = args.prj = NULL;
    while ((c = getopt(argc, argv, ":Aac:hs")) != -1) {
        switch (c) {
        case 'A':
            list_filter.allall = TRUE;
            break;
        case 'a':
            list_filter.almostall = TRUE;
            break;
        case 'c':
            list_filter.column = optarg;
            break;
        case 'h':
            showhelp = TRUE;
            break;
        case 's':
            list_filter.specialonly = TRUE;
            break;
        case ':':
            return elog(1, "option `-%c' requires an argument", optopt);
        default:
            return elog(1, "invalid option `%c'", optopt);
        }
    }

    if (list_filter.specialonly == TRUE)
        list_filter.almostall = FALSE;
    if (list_filter.allall == TRUE)
        list_filter.almostall = FALSE;
    if (list_filter.column != NULL) {
        list_filter.almostall = FALSE;

        // hotfix
        elog(1, "option `-c' not implemented yet");
        return 1;
    }
    // TODO: check that option don't conflict with each other.
    if (showhelp == 1)
        return help_usage("list");

    i = optind;
    do {
        args.prj = argv[i];

        if ((status = tman_task_list(ctx, &args, NULL)) != TMAN_OK) {
            elog(status, errfmt, args.prj, tman_strerror());
            continue;
        }
        // TODO: add hooks
        recursive_tree_print(ctx->ids);
    } while (++i < argc);

    return status;
}
