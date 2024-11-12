#include <string.h>

#include "../core/core.h"

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
    return 1;
}

int tman_cat(int argc, char **argv)
{
    char c;
    int retcode;
    int o_strict = 0;
    char *key = NULL;
    char *env = NULL;

    while ((c = getopt(argc, argv, "e:k:s")) != -1) {
        switch (c) {
            case 'e': env = optarg; break;
            case 'k': key = optarg; break;
            case 's': o_strict = 1; break;
        }
    }

    for (int i = optind; i < argc; ++i) {
        char *id = argv[i];
        retcode = pretty_cat(env, id, key);
        if (o_strict && retcode != 0)
            break;
    }

    /* if no arguments passed cat current task (if any) */
    return optind < argc ? retcode : pretty_cat(env, NULL, key);
}
