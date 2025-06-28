#include <unistd.h>

#include "cli.h"
#include "config.h"

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

static int recursive_tree_print(struct tree *p, struct config *myconfig)
{
    if (p != NULL) {
        char mark = p->mark;
        recursive_tree_print(p->left, myconfig);

        /* Apply filters */
        if (list_filter.specialonly == TRUE && (mark == '*' || mark == '^')) {
            LIST_TASK_UNITS(p);
        } else if (list_filter.almostall == TRUE && (mark != '-')) {
            LIST_TASK_UNITS(p);
        } else if (list_filter.allall == TRUE) {
            LIST_TASK_UNITS(p);
        }

        recursive_tree_print(p->right, myconfig);
    }
    return 0;
}

// TODO: Find a good error message in case option fails.  */
int tman_cli_list(int argc, char **argv, tman_ctx_t * ctx)
{
    char c;
    tman_arg_t args;
    int i, showhelp, showprjname, status;

    showhelp = showprjname = FALSE;
    args.id = args.brd = args.prj = NULL;
    while ((c = getopt(argc, argv, ":Aac:hsvH")) != -1) {
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
        case 'v':
            return elog(1, "option `-v' under development");
        case 'H':
            showprjname = TRUE;
            break;
        case ':':
            return elog(1, "option `-%c' requires an argument", optopt);
        default:
            return elog(1, "invalid option `-%c'", optopt);
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

    tman_pwd_unset();
    i = optind;
    do {
        args.prj = argv[i];

        if ((status = tman_task_list(ctx, &args, NULL)) != LIBTMAN_OK) {
            args.prj = args.prj ? args.prj : "NOCURR";
            elog(status, errfmt, args.prj, tman_strerror());
            continue;
        }

        if (showprjname == TRUE)
            printf("Project: %s\n", args.prj);
        // TODO: add hooks
        recursive_tree_print(ctx->ids, tmancfg);
    } while (++i < argc);

    chdir("/home/roach");
    return status;
}
