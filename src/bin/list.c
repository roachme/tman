#include <string.h>
#include <stdlib.h>

#include "../core/core.h"

static int compare(const void *aa, const void *bb)
{
    struct ilist *a = (struct ilist*)aa;
    struct ilist *b = (struct ilist*)bb;
    return (a->col.level - b->col.level);
}

static int pretty_list(char *env)
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

    return 1;
}

int tman_list(int argc, char **argv)
{
    char c;
    int retcode;

    while ((c = getopt(argc, argv, "a:")) != -1) {
        switch (c) {
            case 'a':
                break ;;
        }
    }

    for (int i = optind; i < argc; ++i) {
        char *env = argv[i];
        retcode = pretty_list(env);
    }

    /* if no arguments passed list current env (if any) */
    return optind < argc ? retcode : pretty_list(NULL);
}
