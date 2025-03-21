#ifndef TMAN_SHOW_H
#define TMAN_SHOW_H

enum tman_cli_show_errno {
    TMAN_CAT_NOID = 20,         /* task ID already exists */
};

struct tman_cli_show_opt {
    char *prj;                  /* project to add a task to */
    char *key;                  /* unit key which value to output */
    int force;                  /* ignore existent task ID */
    int help;                   /* show command usage */
};

#endif
