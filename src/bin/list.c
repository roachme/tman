#include <string.h>
#include <stdlib.h>

#include "list.h"
#include "../core.h"

static int tman_list_usage(void)
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

static int pretty_list(char *env, struct tman_list *opt)
{
    struct list list;

    memset(&list, 0, sizeof(list));
    if (!core_id_list(&list, env)) {
        // error: envname not found
        return 1;
    }

    // sort list according to mark level
    // code goes here..
    qsort((void*)list.ilist, list.num, sizeof(list.ilist[0]), compare);

    for (int i = 0; i < list.num; ++i) {
        struct ilist item = list.ilist[i];
        printf("%c %-10s [%s] %s\n", item.col.mark, item.id, item.pgn, item.desc);
    }

    return TMAN_OK;
}

int tman_list(int argc, char **argv)
{
    char c;
    int help = 0;
    int status;
    struct tman_list opt = { .spec = 1, };

    /*
        -A - list all (even done tasks)
        -a - almost all (everything but done tasks)
        -c - specify what column to list
        -s - default: list only current & previous (maybe?)
    */
    while ((c = getopt(argc, argv, ":Aac:hs")) != -1) {
        switch (c) {
            case 'A':
                opt.all = 1; break;
            case 'a':
                opt.almost = 1; break;
            case 'c':
                opt.col = optarg; break;
            case 'h':
                opt.help = 1; break;
            case 's':
                opt.spec = 1; break;
            case ':':
                return elog(TMAN_INVOPT, "option `-%c' requires an argument", optopt);
            default:
                return elog(TMAN_INVOPT, "invalid option `%c'", optopt);
        }
    }

    // TODO: check that option don't conflict with each other.

    if (opt.help == 1)
        return tman_list_usage();

    for (int i = optind; i < argc; ++i) {
        char *env = argv[i];
        status = pretty_list(env, &opt);
    }

    /* if no arguments passed then list current env */
    return optind < argc ? status : pretty_list(NULL, &opt);
}
