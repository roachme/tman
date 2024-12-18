#include <string.h>

#include "cat.h"
#include "../src/tman.h"

static int tman_cat_usage(void)
{
    const char *cmd = "cat";
    printf("Usage: %s %s [OPTION]... [ID]..\n", PROGRAM, cmd);
    printf("Try '%s help %s' for more info.\n", PROGRAM, cmd);
    return TMAN_OK;
}

static int pretty_cat(char *env, char *id, char *key)
{
    struct unitpgn *tmp;
    struct unitpgn *unitpgn;
    struct units units = { 0 };

    if (core_id_cat(env, id, &units) == NULL) {
        return 1;
    }

    for (int i = 0; i < NKEYS; ++i)
        if (units.bin[i].isset)
            printf("%-7s : %s\n", units.bin[i].key, units.bin[i].val);

    unitpgn = units.pgn;
    while (unitpgn != NULL) {
        printf("%-7s : %s\n", unitpgn->node.key, unitpgn->node.val);
        tmp = unitpgn;
        unitpgn = unitpgn->next;
        unit_delpgn(tmp);
    }
    return TMAN_OK;
}

int tman_cat(int argc, char **argv)
{
    char c;
    int status;
    struct tman_cat_opt opt = { .env = NULL, .help = 0, .force = 0, };

    while ((c = getopt(argc, argv, ":e:fhk")) != -1) {
        switch (c) {
            case 'e': opt.env = optarg; break;
            case 'h': opt.help = 1; break;
            case 'f': opt.force = 1; break;
            case 'k': opt.key = optarg; break;
            case ':':
                return elog(TMAN_INVOPT, "option `-%c' requires an argument", optopt);
            default:
                return elog(TMAN_INVOPT, "invalid option `%c'", optopt);
        }
    }

    if (opt.help == 1)
        return tman_cat_usage();

    for (int i = optind; i < argc; ++i)
        status = pretty_cat(opt.env, argv[i], opt.key);

    /* if no arguments passed cat current task (if any) */
    return optind < argc ? status : pretty_cat(opt.env, NULL, opt.key);
}
