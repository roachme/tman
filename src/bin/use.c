#include "../core/core.h"
#include "../core/common.h"

int tman_use(int argc, char **argv)
{
    char c;
    char *env = NULL, *id = NULL;

    while ((c = getopt(argc, argv, "e:")) != -1) {
        switch (c) {
            case 'e':
                env = optarg; break ;
        }
    }

    if (optind == argc)
        return elog("%s: task id required");

    id = argv[optind];
    if (core_id_use(env, id) != 0)
        return 1;
    return core_currdir();
}

