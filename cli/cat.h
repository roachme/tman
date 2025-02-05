#ifndef TMAN_CAT_H
#define TMAN_CAT_H

enum tman_cli_cat_errno {
    TMAN_CAT_NOID = 20,     /* task ID already exists */
};

struct tman_cli_cat_opt {
    char *prj;      /* project to add a task to */
    char *key;      /* unit key which value to output */
    int force;      /* ignore existent task ID */
    int help;       /* show command usage */
};

#endif
