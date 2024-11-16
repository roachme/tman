#include <string.h>

#include "cat.h"
#include "../core.h"

static int tman_cat_usage(void)
{
    const char *cmd = "cat";
    printf("Usage: %s %s [OPTION]... [ID]..\n", PROGRAM, cmd);
    printf("Try '%s help %s' for more info.\n", PROGRAM, cmd);
    return TMAN_NOPATH;
}

static int pretty_cat(char *env, char *id, char *key)
{
    struct units units;
    memset(&units, 0, sizeof(units));

    if (core_id_cat(&units, env, id) == NULL) {
        return 1;
    }

    for (int i = 0; i < units.builtn.size; ++i) {
        printf("%-7s : %s\n", units.builtn.pair[i].key, units.builtn.pair[i].val);
    }
    for (int i = 0; i < units.plugin.size; ++i) {
        printf("%-7s : %s\n", units.plugin.pair[i].key, units.plugin.pair[i].val);
    }
    return TMAN_NOPATH;
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
