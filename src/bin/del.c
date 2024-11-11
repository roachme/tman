#include "del.h"
#include "../core/common.h"
#include "../core/core.h"

int tman_del(int argc, char **argv)
{
    char c;
    char *env = NULL, *id = NULL;
    int o_strict = 0;
    int o_force = 0;
    int retcode;

    while ((c = getopt(argc, argv, "e:fs")) != -1) {
        switch (c) {
            case 'e':
                env = optarg; break ;
            case 'f':
                o_force = 1;  break ;
            case 's':
                o_strict = 1; break ;
        }
    }

    if (o_force && o_strict)
        return elog("options -f and -s mutually exclusive");

    for (int i = optind; i < argc; ++i) {
        id = argv[i];
        if ((retcode = core_id_del(env, id)) != 0 && o_strict) {
            break;
        }
    }
    if (optind == argc) /* delete current task id */
        retcode = core_id_del(env, id);
    return retcode != 0 ? retcode : core_currdir();
}
